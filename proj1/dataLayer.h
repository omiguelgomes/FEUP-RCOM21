#ifndef APPLAYER_H
#define APPLAYER_H

#define FALSE 0
#define TRUE 1

#define BAUDRATE B38400

/**
 * @brief Opens port and establishes the connection 
 * 
 * @param port Serial port
 * @param role Transmiter or receiver
 * @return int that identifies the data connection or -1 on failure
 */
int llopen(char* port, int role);

/**
 * @brief Finishes connection between transmiter and receiver 
 * 
 * @param fd File descriptor of the serial port
 * @param role Transmiter or receiver
 * @return int positive on success or int negative on failure
 */
int llclose(int fd, int role);

/**
 * @brief Sends the data from the transmiter to the receiver
 * 
 * @param fd File descriptor of the serial port 
 * @param buffer Array of characters to transmit
 * @param length Length of the array to transmit
 * @return int number of written characters on success or int negative on failure
 */
int llwrite(int fd, unsigned char * buffer, int length);

/**
 * @brief receives the data from the transmitter on the receiver side
 * 
 * @param fd File descriptor of the serial port 
 * @param buffer Array of received characters
 * @return int number of read characters on success or int negative on failure 
 */
int llread(int fd, unsigned char* buffer);

#endif
