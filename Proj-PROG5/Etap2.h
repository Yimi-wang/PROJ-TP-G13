#ifndef __ETAP_2_H__
#define __ETAP_2_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>
#include <assert.h>

void etap2(Elf32_Ehdr* ehdr, FILE * fp);

#endif
