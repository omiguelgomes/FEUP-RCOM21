#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "appLayer.h"

#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define RECEIVER 0
#define SENDER 1

volatile int STOP=FALSE;


int main(int argc, char** argv)
{
    int fd, c, res, type;
    char buf[255], str[255];

    /*if ( (argc < 3) || 
        ((strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS1", argv[1])!=0) && (strcmp("/dev/ttyS10", argv[1])!=0) && (strcmp("/dev/ttyS11", argv[1])!=0)) || 
        ((argv[2] != 0) && (argv[2] != 1))){
            printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
            exit(1);
    }*/
    type = atoi(argv[2]);
  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */
    if ((res = llopen(argv[1], type)) != 0){
        printf("Error in llopen\n");
        exit(1);
    }
    llclose(res, type);

    return 0;
}