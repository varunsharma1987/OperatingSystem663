CC=gcc
CFLAGS=-g

mp3: main.o info.o
	$(CC) $(CFLAGS) main.o info.o -o mp3

main.o: main.c info.h
	$(CC) $(CFLAGS) -c main.c -o main.o

info.o: info.c info.h
	$(CC) $(CFLAGS) -c info.c -o info.o

mycp: mycp.c
	$(CC) $(CFLAGS) mycp.c -o mycp

clean: 
	rm -f *.o mp3 mycp
