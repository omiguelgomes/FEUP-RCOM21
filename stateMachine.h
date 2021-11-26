#ifndef STATEMACHINE_H
#define STATEMACHINE_H

typedef enum {
    START,
    FLAG_RCV,
    A_RCV,
    C_RCV,
    BCC_OK,
    STOP} states;


int state_machine(unsigned char byte, states *state, int type);

#endif