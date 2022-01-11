#ifndef __READ_DATA_AUXILIARYIES_H__
#define __READ_DATA_AUXILIARYIES_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <elf.h>
#include <assert.h>

uint32_t reversebytes_Big_2_Little(uint32_t value);

/* Trouver l'indice de la section voulue */
int chercher_index_de_section(Elf32_Shdr *shdr, char *shstrtab, int size, char *nom_sec_but);

/* Calculer le nombre d'entrées d'une section spécifique */
int calcul_nombre_entrees(Elf32_Shdr *shdr, int index_section);

/* Lire les données de l’en-tête (ELF Header) */
Elf32_Ehdr *read_header_ELF(FILE *fp, int flag_Big_Endian);

/* Lire les données de la table de sections (Section Header) */
Elf32_Shdr *read_Section_header(FILE *fp, Elf32_Ehdr *ehdr);

/* Lire les données de '.symtab' */
Elf32_Sym *read_sym_tab(FILE *fp, Elf32_Shdr *shdr, int index, int nb_entree);

/* Lire les données de '.rel.text' */
Elf32_Rel *read_rel_tab(FILE *fp, Elf32_Shdr *shdr, int index, int nb_entree);

/* Lire les données (bruts) d'une section non spécifique (comme '.text', '.data'...) */
uint8_t *read_section(FILE *fp, Elf32_Shdr *shdr, int index);

#endif
