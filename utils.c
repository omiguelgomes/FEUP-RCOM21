#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "alarme.h"
#include "stateMachine.h"
#include "dataLayer.h"

int r = 1;
int s = 0;
extern int flag;

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
    set[3] = (set[1] ^ set[2]);
    set[4] = FLAG;
}

unsigned char create_BCC2(unsigned char* data, int dataLength){
    unsigned char BCC2 = data[0];

    for(int i = 1; i < dataLength; i++){
        BCC2 = (BCC2 ^ data[i]);
    }

    return BCC2;
}

int send_frame(unsigned char *frame, int fd, int length)
{   
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
    unsigned char buf[5];

    while (state != STOP && !flag) {
        res = read(fd, buf, 1);
        if(res == -1) {
            perror("Error reading frame");
            exit(-1);
        }
        else if(res > 0){
            state_machine(buf[0], &state, type);
        }
    }

    if(flag) return -1;
    return 0;
}

int stuffing(unsigned char *buf, int size, unsigned char BCC2, unsigned char *stuffed)
{
    int stuffedIndex = 0;

    for(int i = 0; i < size; i++, stuffedIndex++)
    {
        switch(buf[i])
        {
            case(0x7e): 
                stuffed[stuffedIndex] = 0x7d;
                stuffed[++stuffedIndex] = 0x5e;
                break;
            case(0x7d):
                stuffed[stuffedIndex] = 0x7d;
                stuffed[++stuffedIndex] = 0x5d;
                break;
            default:
                stuffed[stuffedIndex] = buf[i];
        }
    }

    switch (BCC2)
    {
    case (0x7e):
        stuffed[stuffedIndex] = 0x7d;
        stuffed[++stuffedIndex] = 0x5e;
        break;
    case (0x7d):
        stuffed[stuffedIndex] = 0x7d;
        stuffed[++stuffedIndex] = 0x5d;
        break;
    default:
        stuffed[stuffedIndex] = BCC2;
        break;
    }
    return ++stuffedIndex;
}

int destuffing(unsigned char* buf, int size, unsigned char *destuffed)
{
    unsigned char* destuffed_buffer = malloc(size*2);
    int destuffedIndex = 0;

    for(int i = 0; i < size; i++, destuffedIndex++)
    {
        if(buf[i] == 0x7d && buf[i+1] == 0x5e)
        {
            destuffed_buffer [destuffedIndex] = 0x7e;
            i++;
        }    
        else if(buf[i] == 0x7d && buf[i+1] == 0x5d)
        {
            destuffed_buffer [destuffedIndex] = 0x7d;
            i++;
        }
        else {
            destuffed_buffer [destuffedIndex] = buf[i];
        }
    }

    memcpy(destuffed, destuffed_buffer, destuffedIndex);
    free(destuffed_buffer);
    return destuffedIndex;
}

int send_data(int fd, long file_size, FILE *ptr)
{   
    int data_size, sequence_number = 0;
    unsigned char *file = malloc(file_size * sizeof(unsigned char));
    fread(file, sizeof(unsigned char), file_size, ptr);

    for(long i = 0; i < file_size; i += ACTUAL_DATA_SIZE){
        if(file_size - i < ACTUAL_DATA_SIZE){
            data_size = file_size - i + 1;
        }
        else data_size = ACTUAL_DATA_SIZE;

        unsigned char data[data_size];
        memcpy((&data), file + i, data_size);

        int packet_size = data_size + 4;
        unsigned char packet[packet_size];

        create_data_packet(&data, data_size, &packet, packet_size, sequence_number);

        if (llwrite(fd, &packet, packet_size) < 0){
            perror("Error sending data packer;\n");
            exit(-1);
        }

        sequence_number = (sequence_number + 1) % 256;
    }
    fclose(ptr);
    return 0;
}

void create_data_packet(unsigned char* data, int data_size, unsigned char* packet, int packet_size, int sequence_number){
    packet[0] = C_DATA;
    packet[1] = sequence_number;
    packet[2] = data_size / 256;
    packet[3] = data_size % 256;
    memcpy(packet + 4, data, packet_size);
}

int receive_information_frame(int fd, unsigned char* buffer){
    states state = START;
    int res, repeated, data_index = 0, data_size = 0;
    unsigned char buf[1], byte1, byte2, stuffed_data[MAX_STUFFED_SIZE], calculated_BCC2, received_BCC2, frame[5];

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
                if(buf[0] == INFO_CONTROL_BYTE(r)) repeated = 1;
                
                if(buf[0] == INFO_CONTROL_BYTE(1 - r) || repeated){
                    state = C_RCV;
                    byte2 = buf[0];
                }
                else if (buf[0] == FLAG) state = FLAG_RCV;
                else state = START;
                break;

            case C_RCV:
                if(buf[0] == (byte1 ^ byte2)){
                    state = BCC_OK;
                    data_index = 0;
                }
                else if (buf[0] == FLAG) state = FLAG_RCV;
                else state = START;
                break;

            case BCC_OK:
                if(data_index > MAX_STUFFED_SIZE) state = START;
                else if (buf[0] == FLAG){
                    data_size = destuffing(stuffed_data, data_index, buffer);
                    received_BCC2 = buffer[data_size-1];
                    calculated_BCC2 = create_BCC2(buffer, data_size-1);

                    if(repeated){
                        r = 1 - r;
                        create_frame(0, RR, frame);
                        r = 1 - r;
                        send_frame(frame, fd, 5);
                        state = START;
                    }
                    else if (received_BCC2 != calculated_BCC2){
                        printf("SEND_REJ\n");
                        r = 1 - r;
                        create_frame(0, REJ, frame);
                        r = 1 - r;
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
    create_frame(0, RR, frame);
    send_frame(frame, fd, 5);
    r = 1 - r;
    return data_size;
}

int send_information_frame(int fd, unsigned char* buffer, int length){
    if(length > DATA_SIZE){
        perror("Cannot send data that big\n");
        exit(-1);
    }

    unsigned char BCC2 = create_BCC2(buffer, length);

    unsigned char stuffedData[2 * DATA_SIZE + 2];
    int stuffedSize = stuffing(buffer, length, BCC2, stuffedData);
    
    unsigned char frame[stuffedSize + 5];

    frame[0] = FLAG;
    frame[1] = 0x03;
    frame[2] = INFO_CONTROL_BYTE(s);
    frame[3] = (frame[1] ^ frame[2]);

    memcpy(frame + 4, stuffedData, stuffedSize);

    frame[stuffedSize + 4] = FLAG;

    int total = stuffedSize + 5;

    int finish = 0;
    setupAlarm(3, 3);

    send_frame(frame, fd, total);

    while(!finish){
        int ack = receive_ack(fd);
        if(flag){
            printf("Did not receive response, resending frame\n");
            send_frame(frame, fd, total);
            flag = 0;
        }

        if(ack == RR){
            alarm(0);
            finish = 1;
        }
        else if(ack == REJ){
            alarm(3);
            send_frame(frame, fd, total);
            flag = 0;
        }
    }
    s = 1 - s;
    return total;
}

int receive_ack(int fd){
    states state = START;
    int res, i = 0, ack;
    unsigned char buf[1], byte1, byte2;

    while (state != STOP && !flag){
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
                    break;
                
                case FLAG_RCV:
                    byte1 = buf[0];
                    if (byte1 == 0x03) state = A_RCV;
                    else if(byte1 == FLAG) continue;
                    else state = START;
                    break;
                case A_RCV:
                    byte2 = buf[0];
                    if(byte2 == FLAG) state = FLAG_RCV;
                    else if((byte2 & 0x05) == 0x05){
                        ack = RR;
                        state = C_RCV;
                        break;
                    }
                    else if ((byte2 & 0x01) == 0x01){
                        ack = REJ;
                        state = C_RCV;
                        break;
                    }
                    else state = START;
                    break;
                case C_RCV:
                    if(buf[0] == FLAG) state = FLAG_RCV;
                    else if(buf[0] == (byte1 ^ byte2)){
                        state = BCC_OK;
                        break;
                    }
                    else state = START;
                    break;
                case BCC_OK:
                    if(buf[0] == FLAG){
                        state = STOP;
                        return ack;
                    }
                    else state = START;
                    break;  
            }
        }
    }
    if(flag) return -1;
    return 0;
}

int send_control_packet(int fd, unsigned char control_field, long file_size, unsigned char* file_name){
    unsigned char *control = malloc(5 + sizeof(long) + strlen(file_name) + 1);

    control[0] = control_field;
    control[1] = T_SIZE;
    memcpy(control + 3, &file_size, sizeof(long));
    control[2] = (unsigned char) sizeof(long);

    control[sizeof(long) + 3] = T_NAME;
    control[sizeof(long) + 4] = (unsigned char) strlen(file_name) + 1;
    memcpy(control + sizeof(long) + 5, file_name, strlen(file_name) + 1);

    llwrite(fd, control, 5 + sizeof(long) + strlen(file_name) + 1);
    return 0;
}

int read_control_packet(int fd, unsigned char control_field, long* file_size, unsigned char** file_name){
    unsigned char type;
    unsigned char *control = malloc(DATA_SIZE);
    int size, idx = 1, length;

    size = llread(fd, control);

    if(control[0] != control_field){
        perror("Wrong control byte\n");
        exit(-1);
    }

    for(int i = 1; i < size;){
        type = control[i++];
        length = control[i++];

        if(type == T_SIZE){
            memcpy(file_size, control + i, length);
        }
        else if(type == T_NAME){
            *file_name = malloc(length);
            memcpy(*file_name, control + i, length);
        }

        i += length;
    }

    return 0;
}