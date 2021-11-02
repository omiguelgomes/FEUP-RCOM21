#include utils.c
#define RECEIVER 0
#define SENDER 1
#define SET 0
#define DISC 1
#define UA 2
#define RR 3
#define REJ 4
#define FALSE 0
#define TRUE 1

#define FLAG 0x7E


int llopen(int port, int role)
{
    int fd = open(port, O_RDWR | O_NOCTTY);

    if(fd < 0){
        printf("Error opening port %d", port);
        exit(-1);
    }
    if(role == RECEIVER)
    {
        int STOP = FALSE;   
        int i = 0;
        while (STOP == FALSE) {       /* loop for input */
          res = read(fd, buf, 1);
          if(res == -1) {
            STOP = TRUE;
            break;
          }
          else if(res > 0){
            str[i] = buf[0];
            if (buf[0]=='\0' || i == 5) STOP=TRUE;
            i++;
          }
    }
    printf("String: %s\n", str);
    }
    else if(role == SENDER)
    {
        char[5] set = create_trama(role, SET);
        if(write(fd, set, 5) == -1)
        {
            printf("Something went wrong while writing\n");
            return -1;
        }
    }
    return 0;
}
