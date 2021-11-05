#ifndef STATEMACHINE_H
#define STATEMACHINE_H

typedef enum {
    START,
    FLAG_RCV,
    A_RCV,
    C_RCV,
    BCC_OK,
    STOP
} set_states;

void set_state_machine(char byte, set_states *state);

#endif