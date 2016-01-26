# Makefile for UD CISC user-level thread library

CC = gcc
CFLAGS = -g -lmcheck

LIBOBJS = t_lib.o list.o

TSTOBJS = test01a.o test02.o test04.o test07.o 

# specify the executable 

EXECS = test01a test02 test04 test07

# specify the source files

LIBSRCS = t_lib.c list.c

TSTSRCS = test01a.c test02.c test04.c test07.c
TSTEXES = test01a test02 test04 test07
TSTOUTS = 1a.txt 2.txt 4.txt 7.txt 

# ar creates the static thread library

t_lib.a: ${LIBOBJS} Makefile
	ar rcs t_lib.a ${LIBOBJS}

# here, we specify how each file should be compiled, what
# files they depend on, etc.

test: ${TSTEXES} ${TSTOUTS}
	./test01a | diff 1a.txt - > /dev/null && echo "test01a passed~" || echo "test01a FAILED!"
	./test02 | diff 2.txt - > /dev/null && echo "test02 passed~" || echo "test02 FAILED!"
	./test04 | diff 4.txt - > /dev/null && echo "test04 passed~" || echo "test04 FAILED!"
	./test07 | diff 7.txt - > /dev/null && echo "test07 passed~" || echo "test07 FAILED!"

t_lib.o: t_lib.c t_lib.h Makefile
	${CC} ${CFLAGS} -c t_lib.c

list.o:	list.h list.c Makefile
	${CC} ${CFLAGS} -c list.c

test01a.o: test01a.c ud_thread.h Makefile
	${CC} ${CFLAGS} -c test01a.c

test02.o: test02.c ud_thread.h Makefile
	${CC} ${CFLAGS} -c test02.c

test04.o: test04.c ud_thread.h Makefile
	${CC} ${CFLAGS} -c test04.c

test0.o: test07.c ud_thread.h Makefile
	${CC} ${CFLAGS} -c test07.c

test01a: test01a.o t_lib.a Makefile
	${CC} ${CFLAGS} test01a.o t_lib.a -o test01a

test02: test02.o t_lib.a Makefile
	${CC} ${CFLAGS} test02.o t_lib.a -o test02

test04: test04.o t_lib.a Makefile
	${CC} ${CFLAGS} test04.o t_lib.a -o test04

test07: test07.o t_lib.a Makefile
	${CC} ${CFLAGS} test07.o t_lib.a -o test07

clean:
	rm -f t_lib.a ${EXECS} ${LIBOBJS} ${TSTOBJS} 
