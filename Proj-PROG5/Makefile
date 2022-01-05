CC=gcc
CFLAGS=-Wall -Werror
SOURCES=$(wildcard *.c)
OBJ=$(patsubst %.c, %.o, $(SOURCES))

all: Pharse1

clean:
	rm *.o

Pharse1: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

Etap1.o: Etap1.h
Etap2.o: Etap2.h
Etap3.o: Etap3.h
Etap4.o: Etap4.h
Etap5.o: Etap5.h 
Pharse1.o: Etap1.h Etap2.h Etap3.h Etap4.h Etap5.h