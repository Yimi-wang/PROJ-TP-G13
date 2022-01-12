#ifndef __ETAP_5_H__
#define __ETAP_5_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <elf.h>
#include <assert.h>

void etap5(Elf32_Ehdr* ehdr, FILE * fp, int flag);

#endif
