#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include "utils.h"
#include "appLayer.h"
#include "stateMachine.h"

int llopen(char* port, int role)
{
    struct termios oldtio, newtio;

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
    leitura do(s) pr�ximo(s) caracter(es)
  */

    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    if(role == RECEIVER)
    {
      set_states state = START;   
      int res;
      char buf[255], str[255];

      while (state != STOP) {       /* loop for input */
        res = read(fd, buf, 1);
        if(res == -1) {
          printf("Error while reading\n");
          exit(-1);
        }
        else if(res > 0){
          set_state_machine(buf[0], &state);
        }
      }
      printf("everything ok\n");
    }
    else if(role == SENDER)
    {
      unsigned char set[5];
      create_trama(role, SET, set);
      if(write(fd, set, 5) == -1)
      {
          printf("Something went wrong while writing\n");
          exit(-1);
      }
    }

    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);

    return 0;
}
