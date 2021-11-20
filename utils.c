#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "alarme.h"
#include "stateMachine.h"
#include "appLayer.h"

int r = 0;

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
            set[2] = RR_CONTROL_BYTE(r);
            r = 1 - r;
            break;
        case(REJ):
            set[2] = REJ_CONTROL_BYTE(r);
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

    for(int i = 1; i < dataLength; i++){
        BCC2 = BCC2 ^ data[i];
    }

    //Stuffing
    int stuffedSize;
    unsigned char* stuffedData;
    stuffedSize = stuffing(data, dataLength, stuffedData);

    for(int i = 0; i < stuffedSize; i++){
        frame[4 + i] = stuffedData[i];
    }
    
    frame[stuffedSize + 4] = BCC2;
    frame[stuffedSize + 5] = FLAG;

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
            if (state_machine(buf[0], &state, type) == -1) return 1;
        }
    }
    return 0;
}

char byte_wanted()
{
    return 0x01;
}

int parseFile(char* fileName, unsigned char* buf)
{
    FILE *fp;
    long lSize; 
    char *c;

    fp = fopen (fileName , "r");
    if( !fp ) perror(fileName),exit(1);

    fseek( fp , 0L , SEEK_END);
    lSize = ftell(fp) ;
    rewind(fp);

    for (int i = 0; i < lSize; i++)
    {
        buf[i] = fgetc(fp);
    }

    fclose(fp);

    return lSize;
}

int saveFile(char* buf)
{
    FILE *file = fopen("filename", "w");

    int results = fputs(buf, file);
    if (results == EOF) {
        // Failed to write do error code here.
    }
    fclose(file);
}

int stuffing(char *buf, int size, char *stuffed)
{
    int stuffedIndex = 0;

    for(int i = 0; i < size; i++, stuffedIndex++)
    {
        if(buf[i] == 0x7e)
        {
            stuffed[stuffedIndex] = 0x7d;
            stuffed[++stuffedIndex] = 0x5e;
        }    
        else if(buf[i] == 0x7d)
        {
            stuffed[stuffedIndex] = 0x7d;
            stuffed[++stuffedIndex] = 0x5d;
        }
    }
    return stuffedIndex;
}

int destuffing(char* buf, int size, char* destuffed)
{
    int destuffedIndex = 0;

    for(int i = 0; i < size; i++, destuffedIndex++)
    {
        if(buf[i] == 0x7d && buf[i+1] == 0x5e)
        {
            destuffed[destuffedIndex] = 0x7e;
            i++;
        }    
        else if(buf[i] == 0x7d && buf[i+1] == 0x5d)
        {
            destuffed[destuffedIndex] = 0x7d;
            i++;
        }
    }
    return destuffedIndex;
}

int sendData(int fd, long file_size, unsigned char* file)
{   
    int data_size; 
    for(int i = 0; i < file_size; i += DATA_SIZE){
        if(file_size - i < DATA_SIZE){
            data_size = file_size - i;
        }
        else data_size = DATA_SIZE;
        unsigned char* data_packet = malloc(data_size);
        memcpy(data_packet, file + i, data_size);

        if (llwrite(fd, data_packet, data_size) != (data_size + 6)){
            perror("Error sending data packer;\n");
            exit(-1);
        }
    }
    return 0;
}

int receive_information_frame(int fd, unsigned char* frame){
    states state = START;
    int res, i = 0;
    char buf[1];

    while (state != STOP){
        res = read(fd, buf, 1);
        if (res == -1){
            perror("Error reading information frame");
            exit(-1);
        }
        else if(res > 0){
            frame[i] = buf[0];
            if(state == BCC_OK && buf[0] == FLAG) state = STOP;
            else state_machine(buf[0], &state, INFORMATION);
        }
    }
    return 0;
}

int receiveData(int fd, long file_size){
    return 1;
}

int receive_ack(int fd){
    states state = START;
    int res, i = 0, ack;
    unsigned char buf[1], byte1, byte2;

    while (state != STOP){
        res = read(fd, buf, 1);
        if (res == -1){
            perror("Error reading information frame");
            exit(-1);
        }
        else if(res > 0){
            switch(state)
            {
                case START:
                    if(buf[0] == FLAG) state = FLAG_RCV;
                    else return 0;
                    break;
                
                case FLAG_RCV:
                    byte1 = buf[0];
                    if (byte1 == 0x03) state = A_RCV;
                    else return 0;
                    break;
                case A_RCV:
                    byte2 = buf[0];
                    if(byte2 && 0x05 == 0x05){
                        ack = RR;
                        state = C_RCV;
                        break;
                    }
                    else if (byte2 && 0x01 == 0x01){
                        ack = REJ;
                        state = C_RCV;
                        break;
                    }
                    else return 0;
                    break;
                case C_RCV:
                    if(buf[0] == byte1 ^ byte2){
                        state = BCC_OK;
                        break;
                    }
                    else return 0;
                    break;
                case BCC_OK:
                    if(buf[0] == FLAG){
                        state = STOP;
                        return ack;
                    }
                    else return 0;
                    break;  
            }
        }
    }
    return 0;
}