#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#define RECEIVER 0
#define SENDER 1
#define FLAG 0x7E
#define SET 0
#define DISC 1
#define UA 2
#define RR 3
#define REJ 4
#define INFORMATION 5

#define BIT(n) (1 << (n))

#define INFO_CONTROL_BYTE(s) (BIT(6*(s)) & 0x40)
#define RR_CONTROL_BYTE(r) (BIT(7*(r)) | 0x05)
#define REJ_CONTROL_BYTE(r) (BIT(7*(r)) | 0x01)

#define DATA_SIZE 256
#define ACTUAL_DATA_SIZE (DATA_SIZE - 4)
#define MAX_STUFFED_SIZE (DATA_SIZE * 2 + 2)

#define C_DATA 0x01
#define C_START 0x02
#define C_END 0x03
#define T_SIZE 0
#define T_NAME 1

/**
 * @brief Create a frame object
 * 
 * @param role Flag that indicates wheter it is the sender or receiver
 * @param tramaType Indicates which kind of supervision frame it is
 * @param set Buffer where frame is stored
 */
void create_frame(int role, int tramaType, unsigned char *set);

/**
 * @brief Create a BCC2 object
 * 
 * @param data Data to read
 * @param dataLength Length of the data
 * @return unsigned char value of BCC2
 */
unsigned char create_BCC2(unsigned char* data, int dataLength);

/**
 * @brief Writes frame in the serial port
 * 
 * @param frame Buffer that contains the frame
 * @param fd File descriptor of the serial port
 * @param length Lenght of the frame to write
 * @return int 0 on success or -1 on failure
 */
int send_frame(unsigned char *frame, int fd, int length);

/**
 * @brief Reads the received frame
 * 
 * @param fd File descriptor of the serial port
 * @param type Type of supervision trama
 * @return int 0 on success or exit(-1) on failure 
 */
int receive_frame(int fd, int type);


/**
 * @brief Reads the received information frame
 * 
 * @param fd File descriptor of the serial port
 * @param buffer Array of received characters
 * @return int Size of the data on success
 */
int receive_information_frame(int fd, unsigned char* buffer);

/**
 * @brief Sends an information frame
 * 
 * @param fd File descriptor of the serial port
 * @param buffer Array of trasmited characters
 * @param length Length of the data to send
 * @return int 
 */
int send_information_frame(int fd, unsigned char* buffer, int length);

/**
 * @brief Stuffs the data
 * 
 * @param buf Data to stuff
 * @param size Size of the data
 * @param BCC2 Binary Check Character 
 * @param stuffed Stuffed data
 * @return int Stuffed data size
 */
int stuffing(unsigned char* buf, int size, unsigned char BCC2, unsigned char* stuffed);

/**
 * @brief Destuffs the data
 * 
 * @param buf Data to destuff
 * @param size Size of the data
 * @param destuffed Destuffed data
 * @return int Destuffed data size
 */
int destuffing(unsigned char* buf, int size, unsigned char *destuffed);

/**
 * @brief Sends the data
 * 
 * @param fd File descriptor of the serial port
 * @param file_size Size of the file to send
 * @param ptr Pointer to the file
 * @return int 0 on success or -1 on failure
 */
int send_data(int fd, long file_size, FILE *ptr);

/**
 * @brief Create a data packet object
 * 
 * @param data Data to send
 * @param data_size Size of the data
 * @param packet Packet to send
 * @param packet_size Size of the packet
 * @param sequence_number Sequenc number of the packet
 */
void create_data_packet(unsigned char* data, int data_size, unsigned char* packet, int packet_size, int sequence_number);

/**
 * @brief Receives the acknowledgment
 * 
 * @param fd File descriptor of the serial port
 * @return int 0 or -1 if flag is 1
 */
int receive_ack(int fd);

/**
 * @brief Sends a control packet
 * 
 * @param fd File descriptor of the serial port
 * @param control_field Control field of the file
 * @param file_size Size of the file
 * @param file_name Name of the file
 * @return int 0 on success
 */
int send_control_packet(int fd, unsigned char control_field, long file_size, unsigned char* file_name);

/**
 * @brief Reads the control packet
 * 
 * @param fd File descriptor of the serial port
 * @param control_field Control field of the file
 * @param file_size Size of the file
 * @param file_name Name of the file
 * @return int 0 on success or -1 on failure
 */
int read_control_packet(int fd, unsigned char control_field, long* file_size, unsigned char** file_name);

#endif
