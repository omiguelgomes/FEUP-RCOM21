CFLAGS = -g -Wall -Wextra
TARGET = main

all: ret

ret: main.o utils.o appLayer.o stateMachine.o alarme.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o utils.o appLayer.o stateMachine.o alarme.o

main.o: main.c
		gcc -c main.c

utils.o: utils.c
		gcc -c utils.c

appLayer.o: appLayer.c
		gcc -c appLayer.c

stateMachine.o: stateMachine.c
		gcc -c stateMachine.c

alarme.o: alarme.c
		gcc -c alarme.c

clean:
		$(RM) $(TARGET) *.o