#ifndef APPLAYER_H
#define APPLAYER_H

#define FALSE 0
#define TRUE 1

#define BAUDRATE B38400

int llopen(char* port, int role);
int llclose(int fd, int role);
int llwrite(int fd, unsigned char * buffer, int length);
int llread(int fd, unsigned char* buffer);

#endif
