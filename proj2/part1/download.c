#include "download.h"
#include "utils.h"

int parseURL(char *arg, struct url *url)
{
    char prefix[] = "ftp://";
    int urlSize = strlen(arg);
    int i = 0;
    int indexToWrite = 0;
    url_parse_states state = START;

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
    else{
        exit(0);
    }

    struct hostent *h;
    if ((h = gethostbyname(url.host)) == NULL) {
        herror("gethostbyname()");
        exit(-1);
    }

    char* address = inet_ntoa(*((struct in_addr *) h->h_addr));

    int sockfd = create_socket(address);
    
    char reply[3];

    // Welcome message
    reply = read_from_socket(sockfd);

    if(reply[0] == '4' || reply[0] == '5'){  //Transient Negative Completion reply (4) or Permanent Negative Completion reply (5)
        close(sockfd);
        exit(0);
    }

    // Enter user
    

    // Receive password request -> Enter pasword -> Receive successful login OR Receive successful login


    // Enter pasv

    // Receive passv reply
    
    if (close(sockfd) < 0) {
        perror("close()");
        exit(-1);
    }
}

//ftp://[<user>:<password>@]<host>/<url-path>