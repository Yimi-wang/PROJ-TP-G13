#include "Etap7.h"

void etap7(FILE *fp, int index_supprime, char *sec_charge, unsigned int addr) {
	FILE *fp_temp;
	char *temp;
	int count = 0, index_sec_charge = -1, index_sec_symtab = -1, nb_entry_ecrit = 0;
	int a = 0; // Error flag
  	
  	fp_temp = fopen("temp", "w+");
  	
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
	
	uint8_t *data_sec = (uint8_t *)malloc(sizeof(uint8_t)*shdr[index_sec_symtab].sh_size);
	assert(data_sec != NULL);
	a = fread(data_sec, sizeof(uint8_t)*shdr[index_sec_symtab].sh_size, 1, fp);
	assert(a != 0);
	a = fwrite(data_sec, sizeof(uint8_t)*shdr[index_sec_symtab].sh_size, 1, fp_temp);
	assert(a >= 1);
	free(data_sec);
	
	Elf32_Sym sym;
	a = fseek(fp, shdr[index_sec_symtab].sh_offset, SEEK_SET);
	assert(a == 0);
	a = fseek(fp_temp, 0, SEEK_SET);
	assert(a == 0);
	for (int j = 0; j < nb_sym_tab_entry; j++) {
		a = fread(&sym, sizeof(Elf32_Sym), 1, fp_temp);
		assert(a != 0);
		if (sym.st_shndx == index_supprime) 
			continue;
		else if ((sym.st_shndx > index_supprime) && (sym.st_shndx < 0xfff1)) 
			sym.st_shndx = sym.st_shndx - 1;
		
		if (sym.st_shndx == index_sec_charge) 
			sym.st_value = sym.st_value + addr;
		nb_entry_ecrit++;
		a = fwrite(&sym, sizeof(Elf32_Sym), 1, fp);
		assert(a >= 1);
	}
	
	int size_ecrit = nb_entry_ecrit * shdr[index_sec_symtab].sh_entsize;
	if (size_ecrit < shdr[index_sec_symtab].sh_size) {
		printf("incomplète\n");
		uint8_t *temp = (uint8_t *)malloc(sizeof(uint8_t)*1024);
		assert(temp != NULL);
		a = shdr[index_sec_symtab].sh_size - size_ecrit;
		a = fwrite(temp, a, 1, fp);
		assert(a >= 1);
		free(temp);
	}
	
	free(ehdr);
	free(shdr);
	fclose(fp_temp);
}
