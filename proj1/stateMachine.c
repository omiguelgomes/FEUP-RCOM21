#include "stateMachine.h"
#include "utils.h"
#include <stdio.h>

int state_machine(unsigned char byte, states *state, int type)
{
    //save byte 1 and 2 to compare the xor in the end
    unsigned char byte1, byte2;
    switch(*state)
    {
        case START:
            if(byte == FLAG){
                *state = FLAG_RCV;
                return 0;
            }
            break;
        
        case FLAG_RCV:
            byte1 = byte; 
            if(byte == FLAG) return 0;
            else if(byte1 == 0x03 || byte1 == 0x01)
            {
                *state = A_RCV;
                return 0;
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
            if(byte2 == FLAG){
                *state = FLAG_RCV;
                return 0;
            }
            else if(byte2 == byte_expected){
                *state = C_RCV;
                return 0;
            }
            break;
        }
        case C_RCV:
        {   
            if(byte == FLAG){
                *state = FLAG_RCV;
                return 0;
            }
            else if(byte == (byte1 ^ byte2)){
                *state = BCC_OK;
                return 0;
            }
            break;
        }
        case BCC_OK:
            if(byte == FLAG){
                *state = STOP;
                return 0;
            }
            break;  
    }
    *state = START;
    return 0;
}
