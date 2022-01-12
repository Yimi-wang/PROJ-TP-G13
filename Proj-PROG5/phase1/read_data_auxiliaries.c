#include "read_data_auxiliaries.h"

void inverse_tab(uint8_t *p, int start, int nb) {
	uint8_t temp = 0;
	
	if (nb == 2) {
		temp = p[start];
		p[start] = p[start + 1];
		p[start + 1] = temp;
	}else if(nb == 4) {
		temp = p[start];
		p[start] = p[start + 3];
		p[start + 3] = temp;
		
		temp = p[start + 1];
		p[start + 1] = p[start + 2];
		p[start + 2] = temp;
	}
}

void *inverse_data(void *strct, size_t size, char *str) {
	int i = 0;
	uint8_t *data_inverse = (uint8_t *)malloc(size);
	memmove(data_inverse, strct, size);
	
	while (i < size) {
		if (strcmp(str, "ehdr") == 0) {
			if (i == 16 || i == 18 || i >= 40) {
				inverse_tab(data_inverse, i, 2);
				i = i + 2;
			}else if (i >= 20 || i < 40){
				inverse_tab(data_inverse, i, 4);
				i = i + 4;
			}
		}else if (strcmp(str, "Symbol") == 0) {
			if (i == 12 || i == 13) {
				i++;
			}else if(i > 13) {
				inverse_tab(data_inverse, i, 2);
				i = i + 2;
			}else{
				inverse_tab(data_inverse, i, 4);
				i = i + 4;
			}
		}
		else{
			inverse_tab(data_inverse, i, 4);
			i = i + 4;
		}	
	}
	memmove(strct, data_inverse, size);
	free(data_inverse);
	
	return strct;
}

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

Elf32_Ehdr *read_header_ELF(FILE *fp, int flag_Big_Endian) {
	int a = 0;
	Elf32_Ehdr *temp_ehdr = NULL;
	
	temp_ehdr = (Elf32_Ehdr *)malloc(sizeof(Elf32_Ehdr));
	assert(temp_ehdr != NULL);
	a = fseek(fp, 0, SEEK_SET);
	assert(a == 0);
	a = fread(temp_ehdr, sizeof(Elf32_Ehdr), 1, fp);
	assert(a != 0);
	
	if (flag_Big_Endian)  
		inverse_data(temp_ehdr, sizeof(Elf32_Ehdr), "ehdr");
	
	return temp_ehdr;
}	

Elf32_Shdr *read_Section_header(FILE *fp, Elf32_Ehdr *ehdr, int flag_Big_Endian) {
	int a = 0;
	Elf32_Shdr *temp_shdr = NULL;
	
	temp_shdr = (Elf32_Shdr *)malloc(sizeof(Elf32_Shdr)*ehdr->e_shnum);
	assert(temp_shdr != NULL);
	a = fseek(fp, ehdr->e_shoff, SEEK_SET);
	assert(a == 0);
	a = fread(temp_shdr, sizeof(Elf32_Shdr), ehdr->e_shnum, fp);
	assert(a != 0);
	
	if (flag_Big_Endian) {
		for (int i = 0; i < ehdr->e_shnum; i++)
			inverse_data(&temp_shdr[i], sizeof(Elf32_Shdr), "shdr");
	}
	return temp_shdr;
}	

Elf32_Sym *read_sym_tab(FILE *fp, Elf32_Shdr *shdr, int index, int nb_entree, int flag) {
	int a = 0;
	Elf32_Sym *temp_sym_tab = NULL;
	
	temp_sym_tab = (Elf32_Sym *)malloc(sizeof(Elf32_Sym)*nb_entree);
	assert(temp_sym_tab != NULL);
	a = fseek(fp, shdr[index].sh_offset, SEEK_SET);
	assert(a == 0);
	a = fread(temp_sym_tab, shdr[index].sh_size, 1, fp);
	//fread(temp_sym_tab, sizeof(Elf32_Sym), nb_entree, fp);
	assert(a != 0);
	
	if (flag) {
		for (int i = 0; i < nb_entree; i++)
			inverse_data(&temp_sym_tab[i], sizeof(Elf32_Sym), "Symbol");
	}
	
	return temp_sym_tab;
}

Elf32_Rel *read_rel_tab(FILE *fp, Elf32_Shdr *shdr, int index, int nb_entree, int flag) {
	int a = 0;
	Elf32_Rel *temp_rel = NULL;
	
	temp_rel = (Elf32_Rel *)malloc(sizeof(Elf32_Rel)*nb_entree);
	assert(temp_rel != NULL);
	a = fseek(fp, shdr[index].sh_offset, SEEK_SET);
	assert(a == 0);
	a = fread(temp_rel, shdr[index].sh_size, 1, fp);
	assert(a != 0);
	
	if (flag) {
		for (int i = 0; i < nb_entree; i++)
			inverse_data(&temp_rel[i], sizeof(Elf32_Rel), "Relocation");
	}
	
	return temp_rel;
}

Elf32_Rela *read_rela_tab(FILE *fp, Elf32_Shdr *shdr, int index, int nb_entree, int flag) {
	int a = 0;
	Elf32_Rela *temp_rela = NULL;
	
	temp_rela = (Elf32_Rela *)malloc(sizeof(Elf32_Rela)*nb_entree);
	assert(temp_rela != NULL);
	a = fseek(fp, shdr[index].sh_offset, SEEK_SET);
	assert(a == 0);
	a = fread(temp_rela, shdr[index].sh_size, 1, fp);
	assert(a != 0);
	
	if (flag) {
		for (int i = 0; i < nb_entree; i++)
			inverse_data(&temp_rela[i], sizeof(Elf32_Rela), "Relocation_a");
	}
	
	return temp_rela;
}

uint8_t *read_section(FILE *fp, Elf32_Shdr *shdr, int index, int flag) {
	int a = 0;
	uint8_t *temp_sec = NULL;
	
	temp_sec = (uint8_t *)malloc(sizeof(uint8_t)*shdr[index].sh_size);
	assert(temp_sec != NULL);
	a = fseek(fp, shdr[index].sh_offset, SEEK_SET);
	assert(a == 0);
	a = fread(temp_sec, shdr[index].sh_size, 1, fp);
	assert(a != 0);
	
	if (flag)
		inverse_data(temp_sec, shdr[index].sh_size, "normal");
	
	return temp_sec;
}

char *read_sh_str_tab(FILE *fp, Elf32_Shdr *shdr, int index) {
	int a = 0;
	char *temp_shstrtab = (char *)malloc(shdr[index].sh_size);
	
	a = fseek(fp, shdr[index].sh_offset, SEEK_SET);
	assert(a == 0);
	a = fread(temp_shstrtab, shdr[index].sh_size, 1, fp);
	assert(a != 0);	
	
	return temp_shstrtab;
}
