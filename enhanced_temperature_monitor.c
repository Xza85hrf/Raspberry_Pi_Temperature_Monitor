#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "temperature_monitor.h"

#define LOG_SIZE_LIMIT 1048576  // 1 MB limit for log file before rotation
#define MAX_SMOOTHING_SAMPLES 5 // Number of samples for moving average

double temperature_samples[MAX_SMOOTHING_SAMPLES] = {0};
int sample_index = 0;

// Function to log the temperature if it exceeds 65°C
void log_temperature(double temperature, const char* log_file) {
    if (temperature <= 65.0) {
        return;
    }

    rotate_log(log_file);

    FILE* file = fopen(log_file, "a");
    if (file == NULL) {
        fprintf(stderr, "Error opening log file.\n");
        return;
    }
    time_t now = time(NULL);
    struct tm *local_time = localtime(&now);

    // Color coding the log entries based on temperature ranges
    if (temperature <= 40.0) {
        fprintf(file, "\033[0;32m"); // Green for 0-40°C
    } else if (temperature <= 65.0) {
        fprintf(file, "\033[0;33m"); // Yellow for 40-65°C
    } else if (temperature <= 75.0) {
        fprintf(file, "\033[0;35m"); // Orange for 65-75°C
    } else {
        fprintf(file, "\033[0;31m"); // Red for above 75°C
    }

    // Logging the temperature with timestamp
    fprintf(file, "[%04d-%02d-%02d %02d:%02d:%02d] Temperature: %.2f°C\033[0m\n",
            local_time->tm_year + 1900, local_time->tm_mon + 1, local_time->tm_mday,
            local_time->tm_hour, local_time->tm_min, local_time->tm_sec, temperature);

    fclose(file);
}

// Function to check if the temperature exceeds the threshold and notify
void check_temperature(double temperature, double threshold) {
    if (temperature > threshold) {
        printf("\033[0;31mTemperature above threshold! Current temperature: %.2f°C\033[0m\n", temperature);
        // Code to send alert (e.g., email, SMS, system notification) can be added here
    }
}

// Function to read the current temperature using the vcgencmd command
double read_temperature() {
    FILE *fp = popen("/usr/bin/vcgencmd measure_temp", "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to run command\n");
        return -1;
    }

    double temperature = 0.0;
    if (fscanf(fp, "temp=%lf'C", &temperature) != 1) {
        fprintf(stderr, "Error reading temperature\n");
        pclose(fp);
        return -1;
    }
    pclose(fp);
    return temperature;
}

// Function to daemonize the process
void daemonize() {
    pid_t pid, sid;
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
    umask(0);
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }
    if ((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

// Function to rotate the log file if it exceeds the size limit
void rotate_log(const char* log_file) {
    struct stat st;
    if (stat(log_file, &st) == 0 && st.st_size >= LOG_SIZE_LIMIT) {
        char backup_log[256];
        snprintf(backup_log, sizeof(backup_log), "%s.bak", log_file);
        rename(log_file, backup_log);
    }
}

// Function to calculate a moving average of the temperature to smooth out rapid changes
double moving_average(double new_temp) {
    temperature_samples[sample_index % MAX_SMOOTHING_SAMPLES] = new_temp;
    sample_index++;
    double sum = 0.0;
    int count = (sample_index < MAX_SMOOTHING_SAMPLES) ? sample_index : MAX_SMOOTHING_SAMPLES;
    for (int i = 0; i < count; i++) {
        sum += temperature_samples[i];
    }
    return sum / count;
}

// Signal handler for graceful termination
void handle_signal(int signal) {
    printf("Terminating...\n");
    exit(0);
}

// Main function to handle program execution
int main(int argc, char* argv[]) {
    const char* log_file = "temperature_log.txt";
    double threshold = 80.0;
    int poll_interval = 1;
    int run_as_daemon = 0;

    int opt;
    // Parsing command line arguments
    while ((opt = getopt(argc, argv, "t:l:i:d")) != -1) {
        switch (opt) {
            case 't':
                threshold = atof(optarg);
                break;
            case 'l':
                log_file = optarg;
                break;
            case 'i':
                poll_interval = atoi(optarg);
                break;
            case 'd':
                run_as_daemon = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-t threshold] [-l log_file] [-i poll_interval] [-d]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (run_as_daemon) {
        daemonize();
    }

    signal(SIGINT, handle_signal); // Handle Ctrl+C to terminate gracefully

    printf("Measuring temperature...\n");
    printf("-----------------------\n");

    while (1) {
        double temperature = read_temperature();
        if (temperature < 0) {
            usleep(1000000); // Sleep for 1 second before retrying
            continue;
        }

        double smoothed_temperature = moving_average(temperature);

        // Display the temperature in the terminal with color coding
        if (smoothed_temperature <= 40.0) {
            printf("\033[0;32m"); // Green for 0-40°C
        } else if (smoothed_temperature <= 65.0) {
            printf("\033[0;33m"); // Yellow for 40-65°C
        } else if (smoothed_temperature <= 75.0) {
            printf("\033[0;35m"); // Orange for 65-75°C
        } else {
            printf("\033[0;31m"); // Red for above 75°C
        }
        printf("Current temperature: %.2f°C\033[0m\n", smoothed_temperature);

        // Log and check the temperature
        log_temperature(smoothed_temperature, log_file);
        check_temperature(smoothed_temperature, threshold);

        // Use nanosleep for precise sleep intervals
        struct timespec ts;
        ts.tv_sec = poll_interval;
        ts.tv_nsec = 0;
        nanosleep(&ts, NULL);
    }

    return 0;
}
