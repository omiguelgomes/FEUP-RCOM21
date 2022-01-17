#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#define SERVER_PORT 6000
#define SERVER_ADDR "192.168.28.96"


struct url {
    char *user;    
    char *password;
    char *host;
    char *path;
};

typedef enum {
    START,
    PREFIX_OK,
    USER_OK,
    PASS_OK,
    HOST_OK,
} url_parse_states;

typedef enum {
    USER_AND_PASS,
    USER,
    NONE
} url_type_states;

#endif