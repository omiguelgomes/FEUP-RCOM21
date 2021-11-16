#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "utils.h"
#include "alarme.h"
#include "stateMachine.h"


void create_frame(int role, int tramaType, unsigned char *set)
{
    set[0] = FLAG;
    //comands sent by sender, or returns by receiver
    if((role == SENDER && tramaType <= 1) || (role == RECEIVER && tramaType > 1))
    {
        set[1] = 0x03;
    }
    else
    {
        set[1] = 0x01;
    }
    switch(tramaType)
    {
        case(SET):
            set[2] = 0x03;
            break;
        case(DISC):
            set[2] = 0x0B;
            break;
        case(UA):
            set[2] = 0x07;
            break;
        case(RR):
            break;
        case(REJ):
            break;
    }
    set[3] = set[1] ^ set[2];
    set[4] = FLAG;
}

int create_information_frame(unsigned char* frame, unsigned char control, unsigned char* data, int dataLength){
    frame[0] = FLAG;
    frame[1] = 0x03;
    frame[2] = control;
    frame[3] = frame[1] ^ frame[2];

    unsigned char BCC2 = data[0];
    frame[4] = data[0];
    
    if(dataLength > 0){
        for(int i = 1; i < dataLength; i++){
            frame[i + 4] = data[i];
            BCC2 = BCC2 ^ data[i];
        }
    }
    
    frame[dataLength + 4] = BCC2;
    frame[dataLength + 5] = FLAG;

    return 0;
}

int send_frame(unsigned char *frame, int fd, int length)
{
    /*for (int i = 0; i < length; i++){
        printf("SEND_FRAME: %x\n", frame[i]);
    }*/
    if(write(fd, frame, length) == -1){
        perror("Error sending frame");
        exit(-1);
    }
    return 0;
}

int receive_frame(int fd, int type)
{
    states state = START;   
    int res;
    char buf[5];

    while (state != STOP) {
        res = read(fd, buf, 1);
        if(res == -1) {
            perror("Error reading frame");
            exit(-1);
        }
        else if(res > 0){
            //printf("RECEIVED_FRAME: %x\n", buf[0]);
            state_machine(buf[0], &state, type);
        }
    }
    
    return 0;
}

int parseFile(char* fileName, unsigned char* buf)
{
    FILE *fptr;

     if ((fptr = fopen(fileName ,"rb")) == NULL){
       printf("Error! opening file");

       // Program exits if the file pointer returns NULL.
       exit(1);
   }

    fscanf(fptr,"%x", buf); 

    fclose(fptr);
}