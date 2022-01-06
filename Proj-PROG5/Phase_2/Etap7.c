#include "Etap7.h"

void corrige_ndx(FILE *fp, int index_supprime) {
	char *temp_str;
	int count = 0, index_sec_symtab = -1;
	int a = 0; // Error flag
	
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)malloc(sizeof(Elf32_Ehdr));
	a = fseek(fp, 0, SEEK_SET);
	assert(a == 0);
	a = fread(ehdr, sizeof(Elf32_Ehdr), 1, fp);
	assert(a != 0);

	count = ehdr->e_shnum;
	
	Elf32_Shdr *shdr = (Elf32_Shdr *)malloc(sizeof(Elf32_Shdr)*count);
	assert(shdr != NULL);
	a = fseek(fp, ehdr->e_shoff, SEEK_SET);
	assert(a == 0);
	a = fread(shdr, sizeof(Elf32_Shdr), count, fp);
	assert(a != 0);
    
	char shstrtab[shdr[ehdr->e_shstrndx].sh_size];
	a = fseek(fp, shdr[ehdr->e_shstrndx].sh_offset, SEEK_SET);
	assert(a == 0);
	a = fread(shstrtab, shdr[ehdr->e_shstrndx].sh_size, 1, fp);
	assert(a != 0);
	
	for (int i = 0; i < count; i++) {
		temp_str = shstrtab;
		temp_str = temp_str + shdr[i].sh_name;
		if (shdr[i].sh_type == 2) //SHT_SYMTAB = 2
			index_sec_symtab = i;
	}
	
	assert(shdr[index_sec_symtab].sh_entsize != 0);
	int nb_sym_tab_entry = shdr[index_sec_symtab].sh_size / shdr[index_sec_symtab].sh_entsize;
	
	a = fseek(fp, shdr[index_sec_symtab].sh_offset, SEEK_SET);
	assert(a == 0);
	
	Elf32_Sym *symdr = (Elf32_Sym *)malloc(sizeof(Elf32_Sym)*nb_sym_tab_entry);
	Elf32_Sym *temp = (Elf32_Sym *)malloc(sizeof(Elf32_Sym)*nb_sym_tab_entry);
	a = fread(temp, shdr[index_sec_symtab].sh_size, 1, fp);
	assert(a != 0);
	
	a = fseek(fp, shdr[index_sec_symtab].sh_offset, SEEK_SET);
	assert(a == 0);
	int index = 0;
	for (int j = 0; j < nb_sym_tab_entry; j++) {
		if (temp[j].st_shndx != index_supprime) {
			fread(&symdr[index], sizeof(Elf32_Sym), 1, fp);
			if ((temp[j].st_shndx > index_supprime) && (temp[j].st_shndx < 0xfff1))
				symdr[index].st_shndx = symdr[index].st_shndx - 1;
			index++;
		}
	}
	a = fseek(fp, shdr[index_sec_symtab].sh_offset, SEEK_SET);
	assert(a == 0);
	a = fwrite(symdr, sizeof(uint8_t)*shdr[index_sec_symtab].sh_size, 1, fp);
	assert(a >= 1);
	
	free(temp);
	free(symdr);
	free(ehdr);
	free(shdr);
}

int addr_charge(FILE *fp, char *sec_charge, unsigned int addr) {
	char *temp;
	int count = 0, index_sec_charge = -1, index_sec_symtab = -1;
	int a = 0; // Error flag
  	
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)malloc(sizeof(Elf32_Ehdr));
	a = fseek(fp, 0, SEEK_SET);
	assert(a == 0);
	a = fread(ehdr, sizeof(Elf32_Ehdr), 1, fp);
	assert(a != 0);

	count = ehdr->e_shnum;
	
	Elf32_Shdr *shdr = (Elf32_Shdr *)malloc(sizeof(Elf32_Shdr)*count);
	assert(shdr != NULL);
	a = fseek(fp, ehdr->e_shoff, SEEK_SET);
	assert(a == 0);
	a = fread(shdr, sizeof(Elf32_Shdr), count, fp);
	assert(a != 0);
    
	char shstrtab[shdr[ehdr->e_shstrndx].sh_size];
	a = fseek(fp, shdr[ehdr->e_shstrndx].sh_offset, SEEK_SET);
	assert(a == 0);
	a = fread(shstrtab, shdr[ehdr->e_shstrndx].sh_size, 1, fp);
	assert(a != 0);
	
	for (int i = 0; i < count; i++) {
		temp = shstrtab;
		temp = temp + shdr[i].sh_name;
		if (shdr[i].sh_type == 2) //SHT_SYMTAB = 2
			index_sec_symtab = i;
		else if (strcmp(temp, sec_charge) == 0) 
			index_sec_charge = i;			
	}
	
	assert(shdr[index_sec_symtab].sh_entsize != 0);
	int nb_sym_tab_entry = shdr[index_sec_symtab].sh_size / shdr[index_sec_symtab].sh_entsize;
	
	a = fseek(fp, shdr[index_sec_symtab].sh_offset, SEEK_SET);
	assert(a == 0);
	
	Elf32_Sym *symdr = (Elf32_Sym *)malloc(sizeof(Elf32_Sym)*nb_sym_tab_entry);
	a = fread(symdr, sizeof(uint8_t)*shdr[index_sec_symtab].sh_size, 1, fp);
	assert(a != 0);
	
	for (int j = 0; j < nb_sym_tab_entry; j++) {
		if (symdr[j].st_shndx == index_sec_charge) 
			symdr[j].st_value = symdr[j].st_value + addr;
	}
	
	a = fseek(fp, shdr[index_sec_symtab].sh_offset, SEEK_SET);
	assert(a == 0);
	a = fwrite(symdr, sizeof(uint8_t)*shdr[index_sec_symtab].sh_size, 1, fp);
	assert(a >= 1);
	
	free(symdr);
	free(ehdr);
	free(shdr);
	
	return index_sec_charge;
}
