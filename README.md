# Raspberry_Pi_Temperature_Monitor
The Temperature Monitor is a C program for Raspberry Pi 5 and 4B that monitors and logs system temperature in real-time. It features threshold-based logging, automatic log rotation, and daemon mode for background processing. The program provides visual feedback with color-coded terminal output and is easy to configure and use.

## Features

- Real-time temperature monitoring with visual feedback.
- Temperature logging with threshold-based activation.
- Automatic log rotation to manage disk space.
- Option to run as a daemon for background processing.
- Color-coded temperature display in the terminal for easy monitoring.

## Installation

### Prerequisites

- A Unix-like operating system (Linux preferred).
- GCC compiler.
- Access to temperature sensors (e.g., via `vcgencmd` on Raspberry Pi).

### Compiling the Program

Clone the repository and compile the source code:

```bash
git clone https://github.com/Xza85hrf/temperature-monitor.git
cd temperature-monitor
gcc -o enhanced_temperature_monitor enhanced_temperature_monitor.c
And run it with:
./enhanced_temperature_monitor
```

## Configuration

Modify the following parameters according to your needs within the program:

- `threshold`: The temperature threshold above which alerts will be logged.
- `log_file`: Path to the log file.
- `poll_interval`: Time interval in seconds between each temperature read.

## Usage

Run the program from the command line:

```bash
./temperature_monitor [-t threshold] [-l log_file] [-i poll_interval] [-d]
```

### Options

- `-t threshold`: Set the temperature threshold (default is 80.0°C).
- `-l log_file`: Specify a custom log file path.
- `-i poll_interval`: Set the polling interval in seconds.
- `-d`: Run as a daemon.

## Code Overview

The main functionalities are divided among several functions:

- `log_temperature()`: Logs temperatures that exceed the configured threshold.
- `check_temperature()`: Checks if the current temperature exceeds the threshold and logs it if so.
- `read_temperature()`: Reads the current temperature using system-specific commands.
- `daemonize()`: Converts the process to a daemon.
- `rotate_log()`: Rotates the log file when it reaches a specified size limit.
- `moving_average()`: Calculates a moving average of the temperature to smooth out rapid changes.

### Main Functions

#### log_temperature()

Logs the temperature to the specified log file. Uses color coding for different temperature ranges.

#### check_temperature()

Checks if the temperature exceeds the threshold and logs a warning if it does.

#### read_temperature()

Reads the current temperature using the `vcgencmd` command on Raspberry Pi.

#### daemonize()

Converts the program into a daemon for background processing.

#### rotate_log()

Rotates the log file when it exceeds 1 MB to manage disk space.

#### moving_average()

Calculates a moving average of the temperature to smooth out rapid changes.

## Contributing

Contributions to the Temperature Monitor project are welcome. Here’s how you can contribute:

1. Fork the repository.
2. Create a new branch for your feature (`git checkout -b feature/my-new-feature`).
3. Commit your changes (`git commit -am 'Add some feature'`).
4. Push to the branch (`git push origin feature/my-new-feature`).
5. Create a new Pull Request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.
