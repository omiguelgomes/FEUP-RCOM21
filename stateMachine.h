#ifndef STATEMACHINE_H
#define STATEMACHINE_H

typedef enum {
    START,
    FLAG_RCV,
    A_RCV,
    C_RCV,
    BCC_OK,
    STOP} states;

/**
 * @brief Machine state that changes the flags according to the program
 * 
 * @param byte Byte to read
 * @param state Current state
 * @param type Type of supervision trama
 * @return int  0 on success
 */
int state_machine(unsigned char byte, states *state, int type);

#endif