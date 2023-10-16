# Parallel Linear Search

Parallel Linear Search is a program designed to search for a string in a file using multiple processes. The primary process divides the file into segments, each of which is scanned by a child process. The parallelism increases the efficiency of the search, especially for large files.

## Table of Contents

- [Parallel Linear Search](#parallel-linear-search)
  - [Table of Contents](#table-of-contents)
  - [Features](#features)
  - [Installation](#installation)

## Features

- Efficient searching using parallel processing.
- Ability to handle large datasets by dividing the workload amongst multiple child processes.
- Returns instantly upon finding the target string, without waiting for all child processes to finish.
- Proper termination of child processes after the target string is found.

## Installation

1. Clone the repository:

git clone https://github.com/spicy/MultiProcessLinearSearch.git

    Navigate to the directory:

cd MultiProcessLinearSearch

    Compile the program:

make all

Usage

    Start the Parallel Linear Search program with the desired file name, search key, and number of processes:

./multi-search <FILE NAME> <KEY> <NUMBER OF PROCESSES>

For example:

./multi-search strings.txt abcd 10

This instructs the program to search for the string "abcd" in the "strings.txt" file using 10 child processes.

    The program will output the result of the search, indicating whether the string was found or not.

Error Handling

The program is robust against various potential errors:

    Errors during the forking of child processes are promptly detected and handled.
    The wait() function is continuously monitored for any discrepancies during execution.
    System calls are diligently error-checked. In case of any failure, the program will terminate and provide a relevant error message.

Example:

if (pid < 0) {
    perror("fork");
    exit(-1);
}