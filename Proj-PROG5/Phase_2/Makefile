CC=gcc
CFLAGS=-Wall -Werror -g
SOURCES=$(wildcard *.c)
OBJ=$(patsubst %.c, %.o, $(SOURCES))

all: Phase2

Phase2: Etap6.o Etap7.o Phase2.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<
 
Etap6.o: Etap6.h 
Etap7.o: Etap7.h
Phase2.o: Etap6.h Etap7.h

clean: 
	rm -f *.o
