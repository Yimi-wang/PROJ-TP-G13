#include "Etap8_9.h"

void proc_reimplants(FILE *fp, int index_sec_charge, unsigned int addr_charge) {
	char *temp, *str;
	int count = 0, index_rel_sec_charge = -1, index_sec_symtab = -1;
	uint32_t new_addr = 0, ndx_rel_sym = 0;
	uint32_t S = 0, A = 0, P = 0;
	unsigned char rel_type = 0, sym_type = 0;
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
	
	str = (char *)malloc(sizeof(shdr[index_sec_charge].sh_name) + 4 + 1); // + .rel + \0
	strcpy(str, ".rel"); // Négliger le type ".rela"
	temp = shstrtab;
	temp = temp + shdr[index_sec_charge].sh_name;
	strcat(str, temp);
	
	for (int i = 0; i < count; i++) {
		temp = shstrtab;
		temp = temp + shdr[i].sh_name;
		if (strcmp(temp, str) == 0)
			index_rel_sec_charge = i;
		else if (strcmp(temp, ".symtab") == 0)
			index_sec_symtab = i;
	}
	free(str);
	
	/* Lire les données de '.symtab' */
	Elf32_Sym *sym = (Elf32_Sym *)malloc(sizeof(Elf32_Sym)*shdr[index_sec_symtab].sh_size);
	fseek(fp, shdr[index_sec_symtab].sh_offset, SEEK_SET);
	fread(sym, sizeof(uint8_t)*shdr[index_sec_symtab].sh_size, 1, fp);
	
	/* Lire les données de '.text' */
	uint8_t *sec_text = (uint8_t *)malloc(sizeof(uint8_t)*shdr[index_sec_charge].sh_size + addr_charge);
	fseek(fp, shdr[index_sec_charge].sh_offset, SEEK_SET);
	fread(sec_text, sizeof(uint8_t)*shdr[index_sec_charge].sh_size, 1, fp);
	
	// Pousser toutes les données de '.text' en arrière pour 'addr' octets 
	uint32_t index_start = shdr[index_sec_charge].sh_size + addr_charge - 1;
	for (int j = index_start; j >= addr_charge; j--) 
		sec_text[j] = sec_text[j - addr_charge];
	for (int l = 0; l < addr_charge; l++)
		sec_text[l] = 0;	
	
	/* Lire les données de '.rel.text' */
	uint32_t size_rel = shdr[index_rel_sec_charge].sh_size;
	Elf32_Rel *reldr = (Elf32_Rel *)malloc(sizeof(Elf32_Rel)*size_rel);
	fseek(fp, shdr[index_rel_sec_charge].sh_offset, SEEK_SET);
	fread(reldr, sizeof(uint8_t)*shdr[index_rel_sec_charge].sh_size, 1, fp);
	assert(shdr[index_rel_sec_charge].sh_entsize != 0);
	int nb_entry_rel = size_rel / shdr[index_rel_sec_charge].sh_entsize;
	
	/* Réimplantation */
	for (int m = 0; m < nb_entry_rel; m++) {	
		ndx_rel_sym = (reldr[m].r_info >> 8);
		rel_type = (unsigned char) reldr[m].r_info;
		sym_type = sym[ndx_rel_sym].st_info & 0xf;
		// Type R_ARM_ABS*
		if ((rel_type == 2) || (rel_type == 5) || (rel_type == 8)) {
			if ((rel_type == 2) && (sym_type == 2)) // STT_FUNC == 2
				new_addr = 1;
			else
				new_addr = sym[ndx_rel_sym].st_value + A; // A = 0 toujours
		}// Type R_ARM_JUMP24 & R_ARM_CALL
		else if ((rel_type == 28) || (rel_type == 29)) {
			S = sym[ndx_rel_sym].st_value;
			P = addr_charge + reldr[m].r_offset;
			if (sym_type == 2)
				new_addr = 1 - P;
			else
				new_addr = S + A - P; // A = 0 toujours
		}
		//Ecrit dans '.text' (32 bits = 4 octets)
		sec_text[reldr[m].r_offset] = new_addr & 0x000f;
		sec_text[reldr[m].r_offset + 1] = (new_addr & 0x00f0) >> 8;
		sec_text[reldr[m].r_offset + 2] = (new_addr & 0x0f00) >> 16;
		sec_text[reldr[m].r_offset + 3] = (new_addr & 0xf000) >> 24;
	}
	
	/* Est-ce qu'il faut décaler les sections étant suivi de '.text' ? */
	
	free(reldr);
	free(sec_text);
	free(sym);
	free(ehdr);
	free(shdr);
}	
