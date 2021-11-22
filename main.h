#ifndef MAIN_H
#define MAIN_H

#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define RECEIVER 0
#define SENDER 1

/**
 * @brief Sends a file through the serial port
 * 
 * @param fd File descriptor of the serial port
 * @param file_path File path
 * @return int 0 on success
 */
int send_file(int fd, char* file_path);

/**
 * @brief Receives a file through the serial port
 * 
 * @param fd File descriptor of the serial port
 * @return int 0 on success; exits with -1 on failure
 */
int receive_file(int fd);

#endif