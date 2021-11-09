#ifndef UTILS_H
#define UTILS_H

#define RECEIVER 0
#define SENDER 1
#define FLAG 0x7E
#define SET 0
#define DISC 1
#define UA 2
#define RR 3
#define REJ 4

void create_frame(int role, int tramaType, char *set);

#endif
