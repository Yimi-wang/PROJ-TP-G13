#include "Etap6.h"

static unsigned int index_section_str(unsigned int index_str, unsigned int index_cible) {
	unsigned int index_str_after = index_str - 1;
	if (index_str == index_cible) 
		return 0;
	else if (index_str > index_cible)
		return index_str_after;
	else 
		return index_str;
}

int etap6(FILE *fp, char *sec_but) {
	char *temp;
	int count = 0, index_sec_but = -1;
	int a = 0; // Error flag
	Elf32_Shdr cible_shdr;
  	
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
		if (strcmp(temp, sec_but) == 0) {
			index_sec_but = i;
			ehdr->e_shstrndx = index_section_str(ehdr->e_shstrndx, i);
  			ehdr->e_shnum--;
			break;
		}
	}	
  	
  	/* Ré-écrit l’en-tête */
  	a = fseek(fp, 0, SEEK_SET);
  	assert(a == 0);
  	a = fwrite(ehdr, sizeof(Elf32_Ehdr), 1, fp);
  	assert(a >= 1);
  	
  	/* Ré-écrit la table des sections */
  	fseek(fp, ehdr->e_shoff, SEEK_SET);
  	for (int j = 0; j < count; j++) {
  		temp = shstrtab;
		temp = temp + shdr[j].sh_name;
		if (strcmp(temp, sec_but) == 0) {
			cible_shdr = shdr[j];
			continue;
		}	
		Elf32_Shdr one_shdr = shdr[j];
		one_shdr.sh_link = index_section_str(one_shdr.sh_link, index_sec_but);
		if (one_shdr.sh_type == 4 || one_shdr.sh_type == 9)
			one_shdr.sh_info = index_section_str(one_shdr.sh_info, index_sec_but);
			
		fwrite(&one_shdr, sizeof(Elf32_Shdr), 1, fp);
  	}
  	
  	Elf32_Shdr masque;
	a = fwrite(&masque, sizeof(Elf32_Shdr), 1, fp);
	assert(a >= 1);
	
	/* Ré-écrit la section cible */
	a = fseek(fp, cible_shdr.sh_offset, SEEK_SET);
	assert(a == 0);
	uint8_t *data_sec = (uint8_t *)malloc(sizeof(uint8_t)*cible_shdr.sh_size);
	a = fwrite(&data_sec, sizeof(cible_shdr.sh_size), 1, fp);
	assert(a >= 1);
	free(data_sec);
	
  	free(ehdr);
	free(shdr);
  	return index_sec_but;
}
