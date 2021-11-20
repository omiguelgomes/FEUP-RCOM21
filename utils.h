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

void create_frame(int role, int tramaType, unsigned char *set);

int create_information_frame(unsigned char* frame, unsigned char control, unsigned char* data, int dataLength);

int send_frame(unsigned char *frame, int fd, int length);

int receive_frame(int fd, int type);

int receive_information_frame(int fd, unsigned char* frame);

int parseFile(char* fileName, unsigned char* buf);

int saveFile(char* buf);

int stuffing(char* buf, int size, char* stuffed);

int destuffing(char* buf, int size, char* destuffed);

char byte_wanted();

int sendData(int fd, long file_size, unsigned char* file);

int receive_ack(int fd);
#endif
