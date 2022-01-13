ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS=-Wall -Werror -g
AM_CFLAGS+=-D DEBUG
AM_CFLAGS+=-D WARNING
AM_CFLAGS+=-D BIG_ENDIAN_SIMULATOR
# Uncomment if performance when running with -DDEBUG is an issue
# Warning, if uncommented, issuing calls to debug functions during options
# parsing might result in debug flag incorrectly set to 0 for some files
#AM_CFLAGS+=-D CACHE_DEBUG_FLAG

LDADD=-lpthread


SUBDIRS=
SUBDIRS+=. Examples_loader

bin_PROGRAMS=
bin_PROGRAMS+=ARM_runner_example
bin_PROGRAMS+=Pharse1
bin_PROGRAMS+=Phase2


COMMON=debug.h debug.c util.h util.c

EXTRA_DIST=filtre.pl
SIMULATOR_INTERFACE=csapp.h csapp.c scanner.h scanner.l gdb_protocol.h gdb_protocol.c arm_simulator_interface.h arm_simulator_interface.c

ARM_runner_example_SOURCES=$(COMMON) $(SIMULATOR_INTERFACE) ARM_runner_example.c
Pharse1_SOURCES=Pharse1.c Etap1.c Etap1.h Etap2.c Etap2.h Etap3.c Etap3.h Etap4.c Etap4.h Etap5.c Etap5.h read_data_auxiliaries.c read_data_auxiliaries.h
Phase2_SOURCES=Phase2.c Etap6.c Etap6.h Etap7.c Etap7.h Etap8_9.c Etap8_9.h Etap11.c Etap11.h read_data_auxiliaries.c read_data_auxiliaries.h

EXTRA_DIST+=sample_run.sh

