#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "alarme.h"

int flag = 0, conta = 1;
int maxTries, timeout;

void atende()                   // atende alarme (alarm handler)
{
   if(conta <= maxTries){
      printf("alarme # %d\n", conta);
      flag = 1;
      conta++;
      alarm(timeout);
   }
   else
   {
      printf("Maximum attempts reached(%i), aborting connection\n", maxTries);
      exit(-1);
   }
}

void setupAlarm(int newMaxTries, int newTimeout)
{
   maxTries = newMaxTries;
   timeout = newTimeout;
   conta = 1;
   (void) signal(SIGALRM, atende);  // instala  rotina que atende interrupcao
   alarm(timeout);
}






/*
int flag = 0, conta = 1;
int maxTries, timeout;

void atende()                   // atende alarme (alarm handler)
{
   flag = 1;
	printf("alarme # %d\n", conta);
	conta++;
   if(conta > maxTries)
   {
      printf("Maximum attempts reached, aborting connection\n");
      exit(-1);
   }
   turnOnAlarm(maxTries, timeout);
}

void setupAlarm()
{
   (void) signal(SIGALRM, atende);  // instala  rotina que atende interrupcao
}

void turnOnAlarm(int newMaxTries, int newTimeout)
{
   maxTries = newMaxTries;
   timeout = newTimeout;
   alarm(timeout);
}

void turnOffAlarm()
{
   flag = 0;
   conta = 1;
   alarm(0);
   return;
}*/