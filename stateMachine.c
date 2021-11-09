#include "stateMachine.h"
#include "utils.h"
#include <stdio.h>

void state_machine(char byte, states *state, int type)
{
    switch(*state)
    {
        case START:
            if(byte == FLAG){
                *state = FLAG_RCV;
                return;
            }
            break;
        
        case FLAG_RCV:
            if(byte == 0x03)
            {
                *state = A_RCV;
                return;
            }
            break;
        
        case A_RCV:
        {
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
            if(byte == byte_expected){
                *state=C_RCV;
                return;
            }
            break;
        }
        case C_RCV:
            if (byte == 0){
                *state = BCC_OK;
                return;
            }
            break;

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
