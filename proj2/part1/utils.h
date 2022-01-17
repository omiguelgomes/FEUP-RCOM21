#ifndef UTILS_H
#define UTILS_H

#define SERVER_PORT 21
#define MAX_LENGTH 200

int create_socket(char* address, int port);
char read_from_socket(int sockfd);
int write_socket(int sockfd, char* cmd, char *args);
int read_pasv(int sockfd);
int download_file(int sockfd, char* path);

#endif