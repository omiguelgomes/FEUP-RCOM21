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

//int s = 0;

extern int flag;

int llopen(char* port, int role)
{
    int res;
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
      turnOnAlarm(3, 3);

      //SEND SET
      unsigned char set[5];
      create_frame(role, SET, set);
      send_frame(set, fd, 5);

      //RECEIVE UA
      res = receive_frame(fd, UA);

      //If nothing is received, resend set, and try to receive UA again
      if(res == 1){
        while(flag == 1)
          { 
            send_frame(set, fd, 5);
            printf("123\n");
            if (receive_frame(fd, UA) == 0) break;
          }
      }
      

      turnOffAlarm();
    }

    return fd;
}

int llwrite(int fd, unsigned char* buffer, int length)
{
  /*int total = length + 6;
  unsigned char* frame[total];
  if (create_information_frame(frame, INFO_CONTROL_BYTE(s), buffer, length) != 0){
    perror("Error in creating information frame.");
    exit(-1);
  }
  
  int ack;
  turnOnAlarm(3, 3);

  send_frame(frame, fd, total);
  do {
    send_frame(frame, fd, total);

    while(flag == 1) {
      send_frame(frame, fd, total);
      
    }

    ack = receive_ack(fd);
  } while (ack != RR);

  turnOffAlarm();
  s = 1 - s;
  return total;*/
  return send_information_frame(fd, buffer, length);
}

int llread(int fd, unsigned char* buffer)
{
  return receive_information_frame(fd, buffer);
}

int llclose(int fd, int role){
  if(role == RECEIVER)
  {
    //RECEIVE DISC
    receive_frame(fd, DISC);

    turnOnAlarm(3, 3);

    //SEND DISC
    unsigned char disc[5];
    create_frame(role, DISC, disc);
    send_frame(disc, fd, 5);

    //RECEIVE UA
    receive_frame(fd, UA);

    //If nothing is received, resend disc, and try to receive UA again
    while(flag == 1)
    { 
      send_frame(disc, fd, 5);
      receive_frame(fd, UA);
    }

    turnOffAlarm();

  }
  else if(role == SENDER)
  {
    //SEND DISC
    unsigned char disc[5];
    create_frame(role, DISC, disc);
    send_frame(disc, fd, 5);

    turnOnAlarm(3, 3);

    //RECEIVE DISC
    receive_frame(fd, DISC);

    //If nothing is received, resend set, and try to receive UA again
    while(flag == 1)
    { 
      send_frame(disc, fd, 5);
      receive_frame(fd, DISC);
    }

    turnOffAlarm();

    //SEND UA
    unsigned char ua[5];
    create_frame(role, UA, ua);
    send_frame(ua, fd, 5);
  }

  /*tcsetattr(fd,TCSANOW,&oldtio);*/
  close(fd);
  return 0;
}