#ifndef STATEMACHINE_H
#define STATEMACHINE_H

typedef enum {
    START,
    FLAG_RCV,
    A_RCV,
    C_RCV,
    BCC_OK,
    STOP} states;


int state_machine(char byte, states *state, int type);
void ack_state_machine(char byte, states *state, int type, int parity);

#endif