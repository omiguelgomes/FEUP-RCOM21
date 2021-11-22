#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "alarme.h"
#include "stateMachine.h"
#include "appLayer.h"

int r = 0;
int s = 0;

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
            break;
        case(REJ):
            set[2] = REJ_CONTROL_BYTE(r);
            break;
    }
    set[3] = set[1] ^ set[2];
    set[4] = FLAG;
}

char create_BCC2(char* data, int dataLength){
    unsigned char BCC2 = data[0];

    for(int i = 1; i < dataLength; i++){
        BCC2 = BCC2 ^ data[i];
    }

    return BCC2;
}

int create_information_frame(unsigned char* frame, unsigned char control, unsigned char* data, int dataLength){
    frame = malloc(dataLength);

    frame[0] = FLAG;
    frame[1] = 0x03;
    frame[2] = control;
    frame[3] = frame[1] ^ frame[2];

    unsigned char BCC2 = create_BCC2(data, dataLength);

    int stuffedSize;
    unsigned char* stuffedData;
    stuffedSize = stuffing(data, dataLength, stuffedData);
    realloc(frame, stuffedSize);

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

int parseFile(char* fileName, unsigned char* buf)
{
    FILE *fp;
    long lSize;

    fp = fopen (fileName , "r");
    if( !fp ) perror(fileName),exit(1);

    fseek( fp , 0L , SEEK_END);
    lSize = ftell(fp) ;
    rewind(fp);

    buf = malloc(lSize);
    fread(buf, sizeof(char), lSize, fp);

    fclose(fp);
    return lSize;
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

int send_data(int fd, long file_size, unsigned char* file)
{   
    int data_size, sequence_number = 0;
    for(long i = 0; i < file_size; i += DATA_SIZE){
        if(file_size - i < DATA_SIZE){
            data_size = file_size - i;
        }
        else data_size = DATA_SIZE;

        char* data = malloc(data_size);
        memcpy(data, file + i, data_size);

        int packet_size = data_size + 4;
        unsigned char* packet = malloc(packet_size);
        create_data_packet(data, data_size, packet, packet_size, sequence_number);

        if (llwrite(fd, packet, packet_size) != (packet_size + 6)){
            perror("Error sending data packer;\n");
            exit(-1);
        }

        sequence_number = (sequence_number + 1) % 256;
        
        free(data);
        free(packet);
    }
    return 0;
}

void create_data_packet(char* data, int data_size, char* packet, int packet_size, int sequence_number){
    packet[0] = C_DATA;
    packet[1] = sequence_number;
    packet[2] = data_size / 256;
    packet[3] = data_size % 256;
    memcpy(packet + 4, data, packet_size);
}

int receive_information_frame(int fd, char* buffer){
    states state = START;
    int res, repeated, data_index, data_size;
    char buf[1], byte1, byte2, stuffed_data[DATA_SIZE], calculated_BCC2, received_BCC2, frame[5];

    while(state != STOP){
        res = read(fd, buf, 1);
        if (res == -1){
            perror("Error reading information frame");
            exit(-1);
        }
        else if (res > 0){
            switch (state)
            {
            case START:
                if(buf[0] == FLAG) state = FLAG_RCV;
                break;

            case FLAG_RCV:
                if(buf[0] == 0x03){
                     state = A_RCV;
                     repeated = 0;
                     byte1 = buf[0];
                }
                else if (buf[0] == FLAG) continue;
                else state = START;
                break;

            case A_RCV:
                if(buf[0] == INFO_CONTROL_BYTE(1 - s)) repeated = 1;
                
                if(buf[0] == INFO_CONTROL_BYTE(s) || repeated){
                    state = C_RCV;
                    byte2 = buf[0];
                }
                else if (buf[0] == FLAG) state = FLAG_RCV;
                else state = START;
                break;

            case C_RCV:
                if(buf[0] == byte1 ^ byte2){
                    state = BCC_OK;
                    data_index = 0;
                }
                else if (buf[0] == FLAG) state = FLAG_RCV;
                else state = START;

            case BCC_OK:
                if(data_index > DATA_SIZE) state = START;
                else if (buf[0] == FLAG){
                    data_size = destuffing(stuffed_data, data_index, buffer);
                    received_BCC2 = buffer[data_size];
                    calculated_BCC2 = create_BCC2(buffer, data_size);

                    if(repeated){
                        create_frame(1, RR, frame);
                        send_frame(frame, fd, 5);
                        state = START;
                    }
                    else if (received_BCC2 != calculated_BCC2){
                        create_frame(1, REJ, frame);
                        send_frame(frame, fd, 5);
                        state = START;
                    }
                    else state = STOP; 
                }
                else stuffed_data[data_index++] = buf[0];
                break;
            }
        }
    }
    create_frame(1, RR, frame);
    send_frame(frame, fd, 5);
    r = 1 - r;
    return data_size;
}

int send_information_frame(int fd, char* buffer, int length){
    if(length > DATA_SIZE){
        perror("Cannot send data that big\n");
        exit(-1);
    }

    int total = length + 6;
    unsigned char* frame = malloc(total);
    if (create_information_frame(frame, INFO_CONTROL_BYTE(s), buffer, length) != 0){
        perror("Error in creating information frame.");
        exit(-1);
    }

    send_frame(frame, fd, total);

    //FALTA RECEIVE_ACK

    s = 1 - s;
    free(frame);
    return total;
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

int saveFile(char* buf)
{
    FILE *file = fopen("filename", "w");

    int results = fputs(buf, file);
    if (results == EOF) {
        // Failed to write do error code here.
    }
    fclose(file);
}

int send_control_packet(int fd, char control_field, long file_size, char* file_name){
    char* control;

    control[0] = control_field;
    control[1] = T_SIZE;
    control[2] = (char) sizeof(file_size);
    memcpy(control + 3, &file_size, sizeof(file_size));

    control[sizeof(file_size) + 3] = T_NAME;
    control[sizeof(file_size) + 4] = (char) strlen(file_name);
    memcpy(control + sizeof(file_size) + 5, file_name, strlen(file_name));

    printf("SCP_2\n");

    llwrite(fd, control, 5 + sizeof(file_size) + strlen(file_name));

    printf("SCP_3\n");

    
    return 0;
}

int read_control_packet(int fd, char control_field, long file_size, char* file_name){
    char *control, type;
    int size, idx = 1, length;
    
    size = llread(fd, control);

    if(control[0] != control_field){
        perror("Wrong control byte\n");
        exit(-1);
    }

    for(int i = 1; i < size;){
        type = control[i++];
        length = atoi(control[i++]);

        if(type == T_SIZE) memcpy(file_size, control + i, length);
        else if(type == T_NAME) memcpy(file_name, control + i, length);

        i += length;
    }

    return 0;
}