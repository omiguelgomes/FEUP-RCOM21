#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "alarme.h"

int flag=1, conta=1, maxTries = 3, resend = 0;

void atende()                   // atende alarme
{
	printf("alarme # %d\n", conta);
	flag = 1;
	conta++;
   resend = 1;
}


int setupAlarm()
{
   (void) signal(SIGALRM, atende);  // instala  rotina que atende interrupcao

   while(conta < 4){
      if(flag){
         alarm(3);                 // activa alarme de 3s
         flag=0;
      }
   }
   printf("Maximum attempts reached, will now shut down\n");
   exit(0);
}

void turnOffAlarm()
{
   flag = 0;
   conta = 1;
   return;
}