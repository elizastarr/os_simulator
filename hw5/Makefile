CC=gcc
CFLAGS=-c -Wall -g  

all: hw5

hw5: simulator.o hw5.o
	$(CC) simulator.o hw5.o -o hw5 -lpthread -lm

simulator.o: simulator.c
	$(CC) $(CFLAGS) simulator.c

hw5.o: hw5.c
	$(CC) $(CFLAGS) hw5.c

clean:
	/bin/rm -f hw5 *.o *.gz

run:
	./hw5

tarball:
	tar -cvzf starr.tar.gz simulator.c simulator.h 

