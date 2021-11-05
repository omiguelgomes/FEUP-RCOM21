#include "stateMachine.h"
#include "utils.h"
#include <stdio.h>

void set_state_machine(char byte, set_states *state)
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
            if(byte == 0x03){
                *state=C_RCV;
                return;
            }
            break;

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
