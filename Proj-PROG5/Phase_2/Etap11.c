#include "Etap11.h"
#include "read_data_auxiliaries.h"

static uint32_t PGCD(uint32_t a, uint32_t b) {
	while(a != b){
		if(a > b)
			a = a - b;
		if(a < b)
			b = b - a;
	}
	
	return a;
}

void produ_executable(FILE *fp, int flag) {
	int a = 0, count = 0;
	int index_sec_data = -1, index_sec_text = -1, index_sec_entry = -1, index_sec_autre = -1;
	uint32_t offset_decal = 0, filesize_base = 0;
	Elf32_Phdr *phdr = NULL;
	FILE *fp_temp = NULL;
	
	fp_temp = fopen("prg1", "w+");
	
	Elf32_Ehdr *ehdr = read_header_ELF(fp, flag);
    
    	count = ehdr->e_shnum;    

    	Elf32_Shdr *shdr = read_Section_header(fp, ehdr, flag);
    	
    	char *shstrtab = read_sh_str_tab(fp, shdr, ehdr->e_shstrndx);
    	
    	index_sec_data = chercher_index_de_section(shdr, shstrtab, count, ".data");
    	assert(index_sec_data != -1);
    	index_sec_text = chercher_index_de_section(shdr, shstrtab, count, ".text");
    	assert(index_sec_text != -1);
    	
    	index_sec_entry = (shdr[index_sec_text].sh_offset < shdr[index_sec_data].sh_offset) ? index_sec_text : index_sec_data;
    	index_sec_autre = (index_sec_entry == index_sec_data) ? index_sec_text : index_sec_data;
    	
    	/* Corriger des points de l'en-tête ELF */
    	ehdr->e_type = 2; // Changer à un fichier exécutable
    	ehdr->e_entry = shdr[index_sec_entry].sh_addr; // l'adresse du segment
    	ehdr->e_phoff = 52; // (octets) Program Header suivi de l'en-tête ELF
    	ehdr->e_flags = 0x5000200;
    	ehdr->e_phentsize = 32; // (octets) = sizeof(Elf32_Phdr)

    	/* '.text' et '.data' sont collées */
    	if (abs(index_sec_data - index_sec_text) == 1) {
    		/* Créer un seul en-tête de programme pour '.text' et '.data' */
    		ehdr->e_phnum = 1;
    		
    		phdr = (Elf32_Phdr *)malloc(sizeof(Elf32_Phdr)*ehdr->e_phnum);
    		assert(phdr != NULL);
    		
    		offset_decal = ehdr->e_phnum * 32;
    		filesize_base = shdr[index_sec_autre].sh_offset - shdr[index_sec_entry].sh_offset;
    		
    		phdr->p_type = 1; // Type = LOAD
    		phdr->p_offset = shdr[index_sec_entry].sh_offset + offset_decal;
    		phdr->p_vaddr = shdr[index_sec_entry].sh_addr;
    		phdr->p_paddr = shdr[index_sec_entry].sh_addr;
    		phdr->p_filesz = filesize_base + shdr[index_sec_autre].sh_size;
    		phdr->p_memsz = phdr->p_filesz;
    		phdr->p_flags = 0x1 + 0x2 + 0x4; // E W R
    		phdr->p_align = PGCD(phdr->p_vaddr, phdr->p_offset);
    	}
    	
    	/* Décalage */
	for (int i = 1; i < count; i++)
		shdr[i].sh_offset = shdr[i].sh_offset + offset_decal;
		
	// Corriger l'offset de la table de sections
	ehdr->e_shoff = ehdr->e_shoff + offset_decal;
 
 	/* Ré-écrit les données & Ecrit le Program Header dans le fichier */
 	
 	// Ré-écrit l'en-tête ELF
 	a = fseek(fp_temp, 0, SEEK_SET);
 	assert(a == 0);
 	my_write(ehdr, sizeof(Elf32_Ehdr), 1, fp_temp, "ehdr", flag);
 	
 	// Ecrit le Program Header
 	a = fseek(fp_temp, 52, SEEK_SET);
 	assert(a == 0);
 	for (int j = 0; j < ehdr->e_phnum; j++)
 		my_write(&phdr[j], sizeof(Elf32_Phdr), 1, fp_temp, "phdr", flag);
 	
 	// Ré-écrit toutes les sections
	uint8_t *bintemp = (uint8_t *)malloc(sizeof(uint8_t)*1024);

	/* Copier les données de la source à la sortie */
	fseek(fp, 52, SEEK_SET);
	while(!feof(fp)) {
		a = fread(bintemp, 1, 1024, fp);
		assert(a != 0);
		if (a != 1024) 
			a = fwrite(bintemp, a, 1, fp_temp);
		else
			a = fwrite(bintemp, 1024, 1, fp_temp);
		assert(a >= 1);
	}
	
	// Ré-écrit la table de sections
 	a = fseek(fp_temp, ehdr->e_shoff, SEEK_SET);
 	assert(a == 0);
 	for (int n = 0; n < count; n++)
 		my_write(&shdr[n], sizeof(Elf32_Shdr), 1, fp_temp, "shdr", flag);

	free(bintemp);
 	free(phdr);
	free(shdr);
	free(ehdr);
}
