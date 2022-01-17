#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "utils.h"

int create_socket(char* address){
    int sockfd;
    struct sockaddr_in server_addr;

    // Server address handling
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(address);           // 32 bit Internet address network byte ordered
    server_addr.sin_port = htons(SERVER_PORT);                  // Server TCP port must be network byte ordered

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
    char code[3];
    char *reply = malloc(1024);
    size_t read;
    size_t n = 0;

    FILE *fp = fdopen(sockfd, "r");
    while((read = getline(&reply, &n, &fp)) != -1){
        if(reply[3] == ' ') break;
    }

    memcpy(&code, &reply, 3);
    return code;
}