CC=gcc
CFLAGS=-Wall -Werror -g
SOURCES=$(wildcard *.c)
OBJ=$(patsubst %.c, %.o, $(SOURCES))

all: Phase2 clean

Phase2: Etap6.o Etap7.o Etap8_9.o Etap11.o Phase2.o read_data_auxiliaries.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<
 
Etap6.o: Etap6.h 
Etap7.o: Etap7.h
Etap8_9.o: Etap8_9.h
Etap11.o: Etap11.h
read_data_auxiliaries.o: read_data_auxiliaries.h
Phase2.o: Etap6.h Etap7.h Etap8_9.h read_data_auxiliaries.h Etap11.h

clean: 
	rm -f *.o
