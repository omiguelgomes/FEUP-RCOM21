#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include "appLayer.h"
#include "alarme.h"

#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define RECEIVER 0
#define SENDER 1

volatile int STOP=FALSE;


int main(int argc, char** argv)
{
  int fd, c, res, type, tramaSize;
  unsigned char buf[65536];


  // if ( (argc < 5) ||
  //     ((strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS1", argv[1])!=0) && (strcmp("/dev/ttyS10", argv[1])!=0) && (strcmp("/dev/ttyS11", argv[1])!=0)) ||
  //     ((argv[2] != 0) && (argv[2] != 1))){
  //         printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
  //         exit(1);
  // }
  type = atoi(argv[2]);
  tramaSize = atoi(argv[3]);

  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */

  struct termios oldtio;
  /*if ((res = llopen(argv[1], type, oldtio)) != 0){
      printf("Error in llopen\n");
      exit(1);
  }*/

  setupAlarm();

  fd = llopen(argv[1], type);

  /*
  if(type == RECEIVER)
  {
    llread();
    saveFile(buf);
  }
  else if(type == SENDER)
  {
    int fileSize = parseFile(argv[4], buf);
    llwrite(fd, buf, fileSize);
  }*/

  llclose(fd, type);

  return 0;
}