#ifndef __ETAP_7_H__
#define __ETAP_7_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <elf.h>
#include <assert.h>

void corrige_ndx(FILE *fp, int index_supprime, int flag);
int addr_charge(FILE *fp, char *sec_charge, uint32_t addr, int flag);

#endif
