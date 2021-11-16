#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include "utils.h"
#include "appLayer.h"
#include "alarme.h"
#include "stateMachine.h"

unsigned char control = '0';

extern int resend, conta;

int llopen(char* port, int role)
{
    struct termios oldtio, newtio;

    //OPEN PORT
    int fd = open(port, O_RDWR | O_NOCTTY);

    if (fd < 0) {
      perror(port);
      exit(-1);
    }

    if (tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 1;   /* inter-character timer */
    newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */
    
    /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) proximo(s) caracter(es)
    */

    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }


    if(role == RECEIVER)
    {
      //RECEIVE SET
      receive_frame(fd, SET);

      //SEND UA
      unsigned char ua[5];
      create_frame(role, UA, ua);
      send_frame(ua, fd, 5);
    }
    else if(role == SENDER)
    {
      setupAlarm();

      //SEND SET
      unsigned char set[5];
      create_frame(role, SET, set);
      send_frame(set, fd, 5);

      //RECEIVE UA
      receive_frame(fd, UA);

      turnOffAlarm();
    }

    return fd;
}

int llwrite(int fd, char * buffer, int length)
{
  int total = length + 6;
  unsigned char* frame[total];
  if (create_information_frame(frame, control, buffer, length) != 0){
    perror("Error in creating information frame.");
    exit(-1);
  }
  send_frame(frame, fd, total);
  return total;
}

int llclose(int fd, int role){
  if(role == RECEIVER)
  {
    //RECEIVE DISC
    receive_frame(fd, DISC);

    //SEND DISC
    unsigned char disc[5];
    create_frame(role, DISC, disc);
    send_frame(disc, fd, 5);

    //RECEIVE UA
    receive_frame(fd, UA);
  }
  else if(role == SENDER)
  {
    //SEND DISC
    unsigned char disc[5];
    create_frame(role, DISC, disc);
    send_frame(disc, fd, 5);

    //RECEIVE DISC
    receive_frame(fd, DISC) != 0;

    //SEND UA
    unsigned char ua[5];
    create_frame(role, UA, ua);
    send_frame(ua, fd, 5);
  }

  /*tcsetattr(fd,TCSANOW,&oldtio);*/
  close(fd);
  return 0;
}