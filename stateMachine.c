#include "stateMachine.h"
#include "utils.h"
#include <stdio.h>

void state_machine(char byte, states *state, int type)
{
    //save byte 1 and 2 to compare the xor in the end
    unsigned char byte1, byte2;
    switch(*state)
    {
        case START:
            if(byte == FLAG){
                *state = FLAG_RCV;
                return;
            }
            break;
        
        case FLAG_RCV:
            byte1 = byte;
            if(byte1 == 0x03 || byte1 == 0x01)
            {
                *state = A_RCV;
                return;
            }
            break;
        
        case A_RCV:
        {
            byte2 = byte;
            unsigned char byte_expected;
            switch(type){
                case SET:
                    byte_expected = 0x03;
                    break;
                case DISC:
                    byte_expected = 0x0B;
                    break;
                case UA:
                    byte_expected = 0x07;
                    break;
            }
            if(byte2 == byte_expected){
                *state = C_RCV;
                return;
            }
            break;
        }
        case C_RCV:
        {   
            if(byte == byte1 ^ byte2){
                *state = BCC_OK;
                return;
            }
            break;
        }
        case BCC_OK:
            if(byte == FLAG){
                *state = STOP;
                return;
            }
            break;  
    }
    *state = START;
    return;
}

void ack_state_machine(char byte, states *state, int type, int parity)
{
    return;
}
