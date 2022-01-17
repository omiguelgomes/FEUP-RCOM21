CFLAGS = -g -Wall -Wextra
TARGET = download

all: ret

ret: download.o utils.o
	$(CC) $(CFLAGS) -o $(TARGET) download.o utils.o

download.o: download.c
		gcc -c download.c

utils.o: utils.c
		gcc -c utils.c

clean:
		$(RM) $(TARGET) *.o