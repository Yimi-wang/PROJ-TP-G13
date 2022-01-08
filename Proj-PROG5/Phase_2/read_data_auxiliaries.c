#include "read_data_auxiliaries.h"

int chercher_index_de_section(Elf32_Shdr *shdr, char *shstrtab, int size, char *nom_sec_but) {
	char *temp = NULL;
	for (int i = 0; i < size; i++) {
		temp = shstrtab;
		temp = temp + shdr[i].sh_name;
		if (strcmp(temp, nom_sec_but) == 0) 
			return i;
	}	
	
	return -1;
}

int calcul_nombre_entrees(Elf32_Shdr *shdr, int index_section) {
	assert(shdr[index_section].sh_entsize != 0);
	return shdr[index_section].sh_size / shdr[index_section].sh_entsize;
}

Elf32_Ehdr *read_header_ELF(FILE *fp) {
	int a = 0;
	Elf32_Ehdr *temp_ehdr = NULL;
	
	temp_ehdr = (Elf32_Ehdr *)malloc(sizeof(Elf32_Ehdr));
	assert(temp_ehdr != NULL);
	a = fseek(fp, 0, SEEK_SET);
	assert(a == 0);
	a = fread(temp_ehdr, sizeof(Elf32_Ehdr), 1, fp);
	assert(a != 0);
	
	return temp_ehdr;
}	

Elf32_Shdr *read_Section_header(FILE *fp, Elf32_Ehdr *ehdr) {
	int a = 0;
	Elf32_Shdr *temp_shdr = NULL;
	
	temp_shdr = (Elf32_Shdr *)malloc(sizeof(Elf32_Shdr)*ehdr->e_shnum);
	assert(temp_shdr != NULL);
	a = fseek(fp, ehdr->e_shoff, SEEK_SET);
	assert(a == 0);
	a = fread(temp_shdr, sizeof(Elf32_Shdr), ehdr->e_shnum, fp);
	assert(a != 0);
	
	return temp_shdr;
}	

Elf32_Sym *read_sym_tab(FILE *fp, Elf32_Shdr *shdr, int index, int nb_entree) {
	int a = 0;
	Elf32_Sym *temp_sym_tab = NULL;
	
	temp_sym_tab = (Elf32_Sym *)malloc(sizeof(Elf32_Sym)*nb_entree);
	assert(temp_sym_tab != NULL);
	a = fseek(fp, shdr[index].sh_offset, SEEK_SET);
	assert(a == 0);
	a = fread(temp_sym_tab, shdr[index].sh_size, 1, fp);
	assert(a != 0);
	
	return temp_sym_tab;
}

Elf32_Rel *read_rel_tab(FILE *fp, Elf32_Shdr *shdr, int index, int nb_entree) {
	int a = 0;
	Elf32_Rel *temp_rel = NULL;
	
	temp_rel = (Elf32_Rel *)malloc(sizeof(Elf32_Rel)*nb_entree);
	assert(temp_rel != NULL);
	a = fseek(fp, shdr[index].sh_offset, SEEK_SET);
	assert(a == 0);
	a = fread(temp_rel, shdr[index].sh_size, 1, fp);
	assert(a != 0);
	
	return temp_rel;
}

uint8_t *read_section(FILE *fp, Elf32_Shdr *shdr, int index) {
	int a = 0;
	uint8_t *temp_sec = NULL;
	
	temp_sec = (uint8_t *)malloc(sizeof(uint8_t)*shdr[index].sh_size);
	assert(temp_sec != NULL);
	a = fseek(fp, shdr[index].sh_offset, SEEK_SET);
	assert(a == 0);
	a = fread(temp_sec, shdr[index].sh_size, 1, fp);
	assert(a != 0);
	
	return temp_sec;
}

