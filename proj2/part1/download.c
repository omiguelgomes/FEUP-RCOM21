#include "download.h"
#include "utils.h"
#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <netdb.h> 
#include <sys/types.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <libgen.h>
#include <unistd.h>
#include <string.h>

int parseURL(char *arg, struct url *url)
{
    char prefix[] = "ftp://";
    int urlSize = strlen(arg);
    int i = 0;
    int indexToWrite = 0;
    url_parse_states state = START;
    url_type_states type_state = NONE;

    //ftp://[<user>:<password>@]<host>/<url-path>

    //username was provided
    if(strstr(arg, "@"))
    {
        //password was provided
        if(strlen(strstr(arg, ":")) != 1)
        {   
            type_state = USER_AND_PASS;
        }
        else{
            type_state = USER;
        }
    }
    else{
        strcpy(url->user, "anonymous");
        type_state = NONE;
    }

    while(i < urlSize)
    {
        switch(state)
        {
            case(START):
                if(arg[i] != prefix[i])
                {
                    return 1;
                }
                else if(i == 5 && arg[i] == prefix[i])
                {
                    state = PREFIX_OK;
                }
                break;

            case(PREFIX_OK):
                if(type_state == NONE)
                {
                    state = PASS_OK;
                    i--;
                    break;
                }
                if(arg[i] != ':')
                {
                    url->user[indexToWrite] = arg[i];
                    indexToWrite++;
                }
                else{
                    state = USER_OK;
                    indexToWrite = 0;
                }
                break;

            case(USER_OK):
                if(type_state == USER)
                {
                    state = PASS_OK;
                    break;
                }
                if(arg[i] != '@')
                {
                    url->password[indexToWrite] = arg[i];
                    indexToWrite++;
                }
                else{
                    state = PASS_OK;
                    indexToWrite = 0;
                }
                break;

            case(PASS_OK):
                if(arg[i] != '/')
                {
                    url->host[indexToWrite] = arg[i];
                    indexToWrite++;
                }
                else{
                    state = HOST_OK;
                    indexToWrite = 0;
                }
                break;

            case(HOST_OK):
                url->path[indexToWrite] = arg[i];
                indexToWrite++;
                break;
        }
        i++;
    }
    printf("- %s\n", url->user);
    printf("- %s\n", url->password);
    printf("- %s\n", url->host);
    printf("- %s\n", url->path);
    return 0;
}

struct url newURL(int length)
{
    struct url url;
    url.host = calloc(1, length);
    url.password = calloc(1, length);
    url.path = calloc(1, length);
    url.user = calloc(1, length);
    return url;
}


int main(int argc, char** argv){

    struct url url = newURL(strlen(argv[1]));

    if(argc != 2 || parseURL(argv[1], &url)){
        fprintf(stderr, "Usage: ./download ftp://[<user>:<password>@]<host>/<url-path>\n");
        exit(1);
    }

    struct hostent *h;
    if ((h = gethostbyname(url.host)) == NULL) {
        herror("gethostbyname()");
        exit(-1);
    }

    char* address = inet_ntoa(*((struct in_addr *) h->h_addr));

    int sockfd = create_socket(address, SERVER_PORT);
    
    char* reply = malloc(3);

    // Welcome message
    reply = read_from_socket(sockfd);

    if(reply == '4' || reply == '5'){  //Transient Negative Completion reply (4) or Permanent Negative Completion reply (5)
        printf("Error in welcome reply.\n");
        close(sockfd);
        exit(-1);
    }

    // Enter user
    write_socket(sockfd, "user ", url.user);
    reply = read_from_socket(sockfd);

    if(reply == '4' || reply == '5'){  //Transient Negative Completion reply (4) or Permanent Negative Completion reply (5)
        printf("Error in user reply.\n");
        close(sockfd);
        exit(-1);
    }
    else if (reply == '3'){
        //Enter password
        write_socket(sockfd, "pass ", url.password);
        reply = read_from_socket(sockfd);

        if(reply == '4' || reply == '5'){  //Transient Negative Completion reply (4) or Permanent Negative Completion reply (5)
            printf("Error in pass reply.\n");
            close(sockfd);
            exit(-1);
        }
    }

    write_socket(sockfd, "pasv", "");

    // Receive & handle pasv reply
    int pasv_sockfd = read_pasv(sockfd);

    write_socket(sockfd, "retr ", url.path);
    reply = read_from_socket(sockfd);

    if(reply == '4' || reply == '5'){  //Transient Negative Completion reply (4) or Permanent Negative Completion reply (5)
        printf("Error in retr reply.\n");
        close(sockfd);
        close(pasv_sockfd);
        exit(-1);
    }

    printf("BEF\n");

    if (download_file(pasv_sockfd, url.path) < 0){
        close(sockfd);
        close(pasv_sockfd);
        exit(-1);
    }

    printf("AFTER\n");

    reply = read_from_socket(sockfd);
    if(reply == '4' || reply == '5'){  //Transient Negative Completion reply (4) or Permanent Negative Completion reply (5)
        printf("Error in download reply.\n");
        close(sockfd);
        close(pasv_sockfd);
        exit(-1);
    }
    
    if (close(sockfd) < 0) {
        perror("close()");
        exit(-1);
    }
    if (close(pasv_sockfd) < 0) {
        perror("close()");
        exit(-1);
    }
}