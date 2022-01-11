CC=gcc
CFLAGS=-Wall -Werror -g
SOURCES=$(wildcard *.c)
OBJ=$(patsubst %.c, %.o, $(SOURCES))

all: Pharse1

Pharse1: Etap1.o Etap2.o Etap3.o Etap4.o Etap5.o Pharse1.o read_data_auxiliaries.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

Etap1.o: Etap1.h
Etap2.o: Etap2.h
Etap3.o: Etap3.h
Etap4.o: Etap4.h
Etap5.o: Etap5.h 
read_data_auxiliaries.o: read_data_auxiliaries.h
Pharse1.o: Etap1.h Etap2.h Etap3.h Etap4.h Etap5.h read_data_auxiliaries.h

clean: 
	rm -f *.o
