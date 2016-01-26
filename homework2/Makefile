CC=gcc
FLAGS=-g
#allflag=-g -lpthread -lkstat -DHAVE_KSTAT
allflag=-g -lpthread -lmcheck -DHAVE_KSTAT 

all: shell.o cycle.o dir.o self.o env.o which.o signal.o exec.o bg.o warnload.o watch.o mail.o rd.o ipc.o
	$(CC) $(allflag) shell.o cycle.o dir.o self.o env.o which.o signal.o exec.o bg.o warnload.o watch.o mail.o rd.o ipc.o -o yuqish

shell: shell.h shell.c
	$(CC) $(FLAGS) -c shell.c -o shell.o

cycle: shell.h cmd.h cycle.c
	$(CC) $(FLAGS) -c cycle.c -o cycle.o

dir: shell.h cmd.h dir.c
	$(CC) $(FLAGS) -c dir.c -o dir.o

self: shell.h cmd.h self.c
	$(CC) $(FLAGS) -c self.c -o self.o

env: shell.h cmd.h env.c
	$(CC) $(FLAGS) -c env.c -o env.o

which: shell.h cmd.h which.c
	$(CC) $(FLAGS) -c which.c -o which.o

signal: shell.h cmd.h sighand.h signal.c
	$(CC) $(FLAGS) -c signal.c -o signal.o

exec: shell.h cmd.h exec.c
	$(CC) $(FLAGS) -c exec.c -o exec.o

bg: util.h shell.h cmd.h bg.c
	$(CC) $(FLAGS) -c bg.c -o bg.o

warnload: util.h shell.h cmd.h warnload.c
	$(CC) $(FLAGS) -c warnload.c -o warnload.o

watch: util.h shell.h cmd.h watch.c
	$(CC) $(FLAGS) -c watch.c -o watch.o
	
mail: util.h shell.h cmd.h mail.c
	$(CC) $(FLAGS) -c mail.c -o mail.o

rd: util.h shell.h cmd.h rd.c
	$(CC) $(FLAGS) -c rd.c -o rd.o

ipc: util.h shell.h cmd.h ipc.c
	$(CC) $(FLAGS) -c ipc.c -o ipc.o

clean:
	rm -f yuqish *.o
