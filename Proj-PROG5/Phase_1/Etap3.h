#ifndef __ETAP_3_H__
#define __ETAP_3_H__

#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <string.h>
#include <assert.h>

void etap3(Elf32_Ehdr* ehdr, FILE * fp, char *str, int flag);
void affichage_contenu_section(int numero, char* temp, Elf32_Shdr *shdr, FILE * fp);

#endif
