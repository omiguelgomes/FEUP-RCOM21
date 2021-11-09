#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include "utils.h"
#include "appLayer.h"
#include "stateMachine.h"

//int alarm

int llopen(char* port, int role)
{
    struct termios oldtio, newtio;

    //OPEN PORT

    int fd = open(port, O_RDWR | O_NOCTTY);

    if (fd < 0) {
      perror(port);
      exit(-1);
    }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 1;   /* inter-character timer unused */
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
      states state = START;   
      int res;
      char buf[255];

      //RECEIVE SET
      while (state != STOP) {
        res = read(fd, buf, 1);
        if(res == -1) {
          printf("Error while reading SET\n");
          exit(-1);
        }
        else if(res > 0){
          state_machine(buf[0], &state, SET);
        }
      }

      printf("AFTER-RECEIVE-SET\n");

      //SEND UA
      unsigned char ua[5];
      create_frame(role, UA, ua);
      if(write(fd, ua, 5) == -1)
      {
          printf("Something went wrong while writing UA\n");
          exit(-1);
      }

    }
    else if(role == SENDER)
    {
      //SEND SET
      unsigned char set[5];
      create_frame(role, SET, set);
      if(write(fd, set, 5) == -1)
      {
          printf("Something went wrong while writing SET\n");
          exit(-1);
      }

      printf("AFTER-SENT-SET\n");

      //RECEIVE UA
      states state = START;
      int res;
      char buf[255];
      while (state != STOP) {
        printf("state: %d\n", state);
        res = read(fd, buf, 1);
        if(res == -1) {
          printf("Error while reading UA\n");
          exit(-1);
        }
        else if(res > 0){
          state_machine(buf[0], &state, UA);
        }
      }
      printf("received UA\n");  
    }

    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);

    return fd;
}

int llclose(int fd, int role){
  if(role == RECEIVER)
  {
      states state = START;
      int res;
      char buf[255];
      //RECEIVE DISC
      while (state != STOP) {
          res = read(fd, buf, 1);
          if(res == -1) {
            printf("Error while reading\n");
            exit(-1);
          }
          else if(res > 0){
            state_machine(buf[0], &state, DISC);
          }
        }

      //SEND DISC
      unsigned char disc[5];
      create_frame(role, DISC, disc);
      if(write(fd, disc, 5) == -1){
          printf("Something went wrong while writing DISC\n");
          exit(-1);
      }

      state = START;
      //RECEIVE UA
      while (state != STOP) {
          res = read(fd, buf, 1);
          if(res == -1) {
            printf("Error while reading\n");
            exit(-1);
          }
          else if(res > 0){
            state_machine(buf[0], &state, UA);
          }
        }

  }
  else if(role == SENDER)
  {
    //SEND DISC
    unsigned char disc[5];
    create_frame(role, DISC, disc);
    if(write(fd, disc, 5) == -1){
        printf("Something went wrong while writing DISC\n");
        exit(-1);
    }

    states state = START;
    int res;
    char buf[255];
    //RECEIVE DISC
    while (state != STOP) {
        res = read(fd, buf, 1);
        if(res == -1) {
          printf("Error while reading\n");
          exit(-1);
        }
        else if(res > 0){
          state_machine(buf[0], &state, DISC);
        }
      }

      //SEND UA
      unsigned char ua[5];
      create_frame(role, UA, ua);
      if(write(fd, ua, 5) == -1)
      {
          printf("Something went wrong while writing UA\n");
          exit(-1);
      }
  }
}