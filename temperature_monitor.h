#ifndef TEMPERATURE_MONITOR_H
#define TEMPERATURE_MONITOR_H

void log_temperature(double temperature, const char* log_file);
void check_temperature(double temperature, double threshold);
double read_temperature();
void daemonize();
void rotate_log(const char* log_file);
double moving_average(double new_temp);

#endif // TEMPERATURE_MONITOR_H
