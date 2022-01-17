#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <libgen.h>
#include "utils.h"

int create_socket(char* address, int port){
    int sockfd;
    struct sockaddr_in server_addr;

    // Server address handling
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(address);           // 32 bit Internet address network byte ordered
    server_addr.sin_port = htons(port);                  // Server TCP port must be network byte ordered

    // Open a TCP socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(-1);
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("connect()");
        exit(-1);
    }

    return sockfd;
}

char read_from_socket(int sockfd){
    char code;
    char *reply = malloc(MAX_LENGTH);
    size_t read;
    size_t n = 0;

    FILE *fp = fdopen(sockfd, "r");
    while((read = getline(&reply, &n, fp)) != -1){
        if(reply[3] == ' ') break;
    }

    code = reply[0];
    return code;
}

int write_socket(int sockfd, char* cmd, char *args){
    write(sockfd, cmd, strlen(cmd));
    write(sockfd, args, strlen(args));
    write(sockfd, "\n", 1);
}

int read_pasv(int sockfd){
    char *reply = malloc(MAX_LENGTH);
    read(sockfd, reply, MAX_LENGTH);

    if(reply[0] == '4' || reply[0] == '5'){
        printf("Error in receiving pasv reply.\n");
        return -1;
    }

    int ip1, ip2, ip3, ip4, port1, port2, port;
    char ip[MAX_LENGTH];
    if(sscanf(reply, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)", &ip1, &ip2, &ip3, &ip4, &port1, &port2) < 0){
        printf("Error in processing pasv reply.\n");
        return -1;
    }

    sprintf(ip, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
    port = port1 * 256 + port2;

    int new_sockfd = create_socket(ip, port);

    return new_sockfd;
}

int download_file(int sockfd, char* path){
    char* name = basename(path);

    FILE *fp = fopen(name, "w");

    if(fp == NULL){
        printf("Error in fopen().\n");
        return -1;
    }

    char buf[1024];
    int bytes_read;
    while((bytes_read = read(sockfd, buf, 1024)) > 0){
        fwrite(buf, bytes_read, 1, fp);
    }

    fclose(fp);

    return 0;
}