CC=gcc
FLAGS=-g

all: list.o main.o
	$(CC) $(FLAGS) list.o main.o -o list

init: list.h list.c
	$(CC) $(FLAGS) -c list.c -o list.o

main: list.h main.c
	$(CC) $(FLAGS) -c main.c -o main.o

clean:
	rm -f *.o list
