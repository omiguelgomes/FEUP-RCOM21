#ifndef UTILS_H
#define UTILS_H

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

unsigned char create_BCC2(unsigned char* data, int dataLength);

/**
 * @brief Create a information frame object
 * 
 * @param frame Buffer where frame is stored
 * @param control Value to place in the 'C' field of the frame
 * @param data Datat to place in the 'Data' field of the frame
 * @param dataLength Length of the data field
 * @return int 0 on success; exits with -1 on failure
 */
int create_information_frame(unsigned char *frame, unsigned char control, unsigned char* data, int dataLength);

/**
 * @brief Writes frame in the serial port
 * 
 * @param frame Buffer that contains the frame
 * @param fd File descriptor of the serial port
 * @param length Lenght of the frame to write
 * @return int 0 on success or -1 on failure
 */
int send_frame(unsigned char *frame, int fd, int length);

int receive_frame(int fd, int type);

int receive_information_frame(int fd, unsigned char* buffer);

int send_information_frame(int fd, unsigned char* buffer, int length);

int parseFile(unsigned char* fileName, unsigned char* buf);

int saveFile(unsigned char* buf);

int stuffing(unsigned char* buf, int size, unsigned char* stuffed);

int destuffing(unsigned char* buf, int size, unsigned char* destuffed);

int send_data(int fd, long file_size, unsigned char* file);

void create_data_packet(unsigned char* data, int data_size, unsigned char* packet, int packet_size, int sequence_number);

int receive_ack(int fd);

int send_control_packet(int fd, unsigned char control_field, long file_size, unsigned char* file_name);

int read_control_packet(int fd, unsigned char control_field, long file_size, unsigned char* file_name);

#endif
