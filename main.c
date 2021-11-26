#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <string.h>
#include "main.h"
#include "appLayer.h"
#include "alarme.h"
#include "utils.h"

/*
TO DO:
  - Alarme (provavelmente ter as tentativas fora e usar o alarm só para o tempo mesmo)
  - Verificar acknowledgments direito (receive_ack())
  - O que fazer quando receber acknowledgments (send_information_frame())
*/


volatile int STOP = FALSE;

int main(int argc, char** argv)
{
  int fd, c, res, type;

  /*if((argc < 4) ||
    (((strcmp("/dev/ttyS0", argv[1]) != 0) && (strcmp("/dev/ttyS1", argv[1])!=0)) || ((strcmp("/dev/ttyS10", argv[1])!=0) && (strcmp("/dev/ttyS11", argv[1])!=0)))){
      printf("Usage: nserial SerialPort\n ex: nserial /dev/ttyS1\n");
      exit(1);
  }*/

  type = atoi(argv[2]);
  if(type != RECEIVER && type != SENDER){
    printf("Application type should be either 0 (Receiver) or 1 (Emitter)\n");
    exit(1);
  }

  setupAlarm();

  fd = llopen(argv[1], type);

  clock_t t;
  t = clock();

  if(type == RECEIVER)
  {
    receive_file(fd);
  }
  else if(type == SENDER)
  {
    if(argc < 4){
      char default_file[11] = "pinguim.gif";
      send_file(fd, &default_file);
    }
    else send_file(fd, argv[3]);
  }

  t = clock() - t;

  llclose(fd, type);

  double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
  printf("Transfer took %f seconds to execute \n", time_taken);

  return 0;
}

int send_file(int fd, unsigned char* file_path){
  unsigned char* file;
  FILE *ptr;

  long file_size;

  ptr = fopen (file_path, "r");
  if(!ptr) perror(file_path),exit(1);

  fseek(ptr, 0, SEEK_END);
  file_size = ftell(ptr) - 1;
  rewind(ptr);

  send_control_packet(fd, C_START, file_size, file_path);

  send_data(fd, file_size, ptr);

  send_control_packet(fd, C_END, file_size, file_path);

  return 0;
}

int receive_file(int fd){
  long file_size;
  unsigned char *file_name = NULL;
  unsigned char buffer[DATA_SIZE];
  FILE *file;
  int sequence_number = 0;

  read_control_packet(fd, C_START, &file_size, &file_name);

  file_name[0] = 'a';

  if((file = fopen(file_name, "w")) == NULL){
      perror("Error creating file");
      exit(-1);
  }

  while (llread(fd, &buffer) > 0){
    if(buffer[0] == C_END) break;
    else if(buffer[0] != C_DATA){
      perror("Invalid control byte\n");
      exit(-1);
    }
    else if(buffer[1] != sequence_number){
      perror("Invalid sequence number\n");
      exit(-1);
    }
    int data_size = buffer[2] * 256 + buffer[3];
    
    unsigned char* data = malloc(data_size);
    memcpy(data, buffer + 4, data_size);

    fwrite(data, sizeof(unsigned char), data_size, file);

    sequence_number = (sequence_number + 1) % 256;
    free(data); 
  }
  free(file_name);
  fclose(file);
  return 0;
}