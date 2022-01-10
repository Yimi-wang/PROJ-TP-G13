#include "Etap6.h"
#include "read_data_auxiliaries.h"

static int renumero_index_section(unsigned int index_original, int index_delete) {
	int index_original_after = index_original - 1;
	if (index_original == index_delete) 
		return 0;
	else if (index_original > index_delete)
		return index_original_after;
	else 
		return index_original;
}

int supprimer_une_section(FILE *fp, char *sec_but) {
	uint8_t octet_null = 0;
	int count = 0, index_sec_but = -1, index_group = 0;
	int a = 0; // Error flag
  	
	Elf32_Ehdr *ehdr = read_header_ELF(fp);

	count = ehdr->e_shnum;
	
	Elf32_Shdr *shdr = read_Section_header(fp, ehdr);
    
	char shstrtab[shdr[ehdr->e_shstrndx].sh_size];
	a = fseek(fp, shdr[ehdr->e_shstrndx].sh_offset, SEEK_SET);
	assert(a == 0);
	a = fread(shstrtab, shdr[ehdr->e_shstrndx].sh_size, 1, fp);
	assert(a != 0);	
	
	index_sec_but = chercher_index_de_section(shdr, shstrtab, count, sec_but);
	assert(index_sec_but >= 0);
	
	/* Corriger les données dans l'en-tête étant sur l'indice d'une section */
	ehdr->e_shstrndx = renumero_index_section(ehdr->e_shstrndx, index_sec_but);
  	ehdr->e_shnum--;
  	
  	/* Ré-écrit l’en-tête */
  	a = fseek(fp, 0, SEEK_SET);
  	assert(a == 0);
  	a = fwrite(ehdr, sizeof(Elf32_Ehdr), 1, fp);
  	assert(a >= 1);
  	
  	/* Ré-écrit la table des sections */
  	fseek(fp, ehdr->e_shoff, SEEK_SET);
  	for (int i = 0; i < count; i++) {
  		// Passer l'entrée de la section cible qui est dans la table de sections
		if (i == index_sec_but) { continue; }	
		
		// Corriger les données d'une entrée si c'est nécessaire
		shdr[i].sh_link = renumero_index_section(shdr[i].sh_link, index_sec_but);
		if (shdr[i].sh_type == 4 || shdr[i].sh_type == 9)
			shdr[i].sh_info = renumero_index_section(shdr[i].sh_info, index_sec_but);
		
		// Ré-écrit une entrée de la table des sections
		fwrite(&shdr[i], sizeof(Elf32_Shdr), 1, fp);
  	}
  	// Compléter la table de sections (à cause de la décalage) par 0
	a = fwrite(&octet_null, 1, sizeof(Elf32_Shdr), fp);
	assert(a >= 1);
	
	/* Couvrir la section cible par 0 */
	a = fseek(fp, shdr[index_sec_but].sh_offset, SEEK_SET);
	assert(a == 0);
	a = fwrite(&octet_null, 1, shdr[index_sec_but].sh_size, fp);
	assert(a >= shdr[index_sec_but].sh_size);
	
	/* Ré-écrit la section '.group' (s'il existe) */
	index_group = chercher_index_de_section(shdr, shstrtab, count, ".group");
	if (index_group != -1) {
		uint8_t *data_sec = read_section(fp, shdr, index_group);
		for (int j = 1; j < shdr[index_group].sh_size; j++) {
			if (data_sec[j] > index_sec_but)
				data_sec[j] = data_sec[j] - 1;
			else if (data_sec[j] == index_sec_but)
				data_sec[j] = 0;
		}
		a = fseek(fp, shdr[index_group].sh_offset, SEEK_SET);
		assert(a == 0);
		a = fwrite(data_sec, shdr[index_group].sh_size, 1, fp);
		assert(a >= 1);
		free(data_sec);
	}

  	free(ehdr);
	free(shdr);
  	return index_sec_but;
}
