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

void create_frame(int role, int tramaType, unsigned char *set);

int create_information_frame(unsigned char* frame, unsigned char control, unsigned char* data, int dataLength);

int send_frame(unsigned char *frame, int fd, int length);

int receive_frame(int fd, int type);

int parseFile(char* fileName, unsigned char* buf);

#endif
