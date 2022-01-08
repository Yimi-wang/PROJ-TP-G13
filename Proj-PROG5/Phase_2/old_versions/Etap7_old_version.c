#include "Etap7.h"
#include "read_data_auxiliaries.h"

void corrige_ndx(FILE *fp, int index_supprime) {
	char *temp_str;
	int count = 0, index_sec_symtab = -1;
	int a = 0; // Error flag
	
	Elf32_Ehdr *ehdr = read_header_ELF(fp);

	count = ehdr->e_shnum;
	
	Elf32_Shdr *shdr = read_Section_header(fp, ehdr);
    
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

int addr_charge(FILE *fp, char *sec_charge, uint32_t addr) {
	char *temp;
	int count = 0, index_sec_charge = -1, index_sec_symtab = -1;
	int a = 0; // Error flag
  	
	Elf32_Ehdr *ehdr = read_header_ELF(fp);

	count = ehdr->e_shnum;
	
	Elf32_Shdr *shdr = read_Section_header(fp, ehdr);
    
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
	
	//Elf32_Sym *symdr = read_sym_tab(fp, shdr, index_sec_symtab);
	
	a = fseek(fp, shdr[index_sec_symtab].sh_offset, SEEK_SET);
	assert(a == 0);
	
	Elf32_Sym *symdr = (Elf32_Sym *)malloc(sizeof(Elf32_Sym)*nb_sym_tab_entry);
	a = fread(symdr, shdr[index_sec_symtab].sh_size, 1, fp);
	assert(a != 0);
	
	for (int j = 0; j < nb_sym_tab_entry; j++) {
		if (symdr[j].st_shndx == index_sec_charge) 
			symdr[j].st_value = symdr[j].st_value + addr;
	}
	
	a = fseek(fp, shdr[index_sec_symtab].sh_offset, SEEK_SET);
	assert(a == 0);
	a = fwrite(symdr, shdr[index_sec_symtab].sh_size, 1, fp);
	assert(a >= 1);
	
	free(symdr);
	free(ehdr);
	free(shdr);
	
	return index_sec_charge;
}
