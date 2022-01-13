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

static void init_phdr(Elf32_Phdr temp_phdr, Elf32_Shdr *temp_shdr, uint32_t off_deca, int index, int textflag) {
	temp_phdr.p_type = 1; // Type = LOAD
	temp_phdr.p_offset = temp_shdr[index].sh_offset + off_deca;
	temp_phdr.p_vaddr = temp_shdr[index].sh_addr;
	temp_phdr.p_paddr = temp_shdr[index].sh_addr;
	temp_phdr.p_filesz = temp_shdr[index].sh_size;
	temp_phdr.p_memsz = temp_phdr.p_filesz;
	if (textflag)
		temp_phdr.p_flags = 0x1 + 0x4; // Execute + Read
	else
		temp_phdr.p_flags = 0x2 + 0x4; // Write + Read
	temp_phdr.p_align = PGCD(temp_phdr.p_vaddr, temp_phdr.p_offset);
}

void produ_executable(FILE *fp, int flag, char *name) {
	int a = 0, count = 0;
	int index_sec_data = -1, index_sec_text = -1, index_sec_entry = -1; 
	int index_sec_autre = -1, index_sec_strtab = -1, index_sec_symtab = -1;
	uint32_t offset_decal = 0, filesize_base = 0, text_offset = 0, data_offset = 0;
	Elf32_Phdr *phdr = NULL;
	char *temp = NULL;
	FILE *fp_temp = NULL;
	
	fp_temp = fopen(name, "w+");
	
	Elf32_Ehdr *ehdr = read_header_ELF(fp, flag);
    
    	count = ehdr->e_shnum;    

    	Elf32_Shdr *shdr = read_Section_header(fp, ehdr, flag);
    	
    	char *shstrtab = read_sh_str_tab(fp, shdr, ehdr->e_shstrndx);
    	
    	index_sec_data = chercher_index_de_section(shdr, shstrtab, count, ".data");
    	index_sec_text = chercher_index_de_section(shdr, shstrtab, count, ".text");
    	index_sec_strtab = chercher_index_de_section(shdr, shstrtab, count, ".strtab");
    	assert(index_sec_strtab != -1);
    	index_sec_symtab = chercher_index_de_section(shdr, shstrtab, count, ".symtab");
    	assert(index_sec_symtab != -1);
    	
    	char *strtab = read_sh_str_tab(fp, shdr, index_sec_strtab);
    	int nb_entry_sym = calcul_nombre_entrees(shdr, index_sec_symtab);
    	
    	Elf32_Sym *sym = read_sym_tab(fp, shdr, index_sec_symtab, nb_entry_sym, flag);
    	
    	if (index_sec_data == -1 && index_sec_text == -1) {
    		printf("Pas de sections pour faire le mapping à segments.\n");
    		return;
    	}else if (index_sec_data == -1 || index_sec_text == -1){
    		index_sec_entry = (index_sec_text == -1) ? index_sec_data : index_sec_text;
    	}else{
    		text_offset = shdr[index_sec_text].sh_offset;
    		data_offset = shdr[index_sec_data].sh_offset;
    		index_sec_entry = (text_offset < data_offset) ? index_sec_text : index_sec_data;
    		index_sec_autre = (index_sec_entry == index_sec_data) ? index_sec_text : index_sec_data;
    	}
    	
    	/* Corriger des points de l'en-tête ELF */
    	ehdr->e_type = 2; // Changer à un fichier exécutable
    	// "Entry" est la valeur (adresse) de la symbole de la fontion "main"
    	for (int k = 0; k < nb_entry_sym; k++) {
    		temp = strtab;
    		temp = temp + sym[k].st_name;
    		if (strcmp(temp, "main") == 0)
    			ehdr->e_entry = sym[k].st_value;
    	}
    	ehdr->e_phoff = 52; 
    	ehdr->e_flags = 0x5000200;
    	ehdr->e_phentsize = 32; // (octets) = sizeof(Elf32_Phdr)

    	/* '.text' et '.data' sont collées */
    	if (abs(index_sec_data - index_sec_text) == 1 || index_sec_autre == -1) {
    		/* Créer un seul en-tête de programme pour '.text' et '.data' */
    		ehdr->e_phnum = 1;
    		
    		phdr = (Elf32_Phdr *)malloc(sizeof(Elf32_Phdr)*ehdr->e_phnum);
    		assert(phdr != NULL);
    		
    		offset_decal = ehdr->e_phnum * 32;
    		if (index_sec_entry != -1 && index_sec_autre != -1) {
    			filesize_base = shdr[index_sec_autre].sh_offset - shdr[index_sec_entry].sh_offset;
    			phdr->p_filesz = filesize_base + shdr[index_sec_autre].sh_size;
    		}else{
    			phdr->p_filesz = shdr[index_sec_entry].sh_size;
    		}
    		
    		
    		phdr->p_type = 1; // Type = LOAD
    		phdr->p_offset = shdr[index_sec_entry].sh_offset + offset_decal;
    		phdr->p_vaddr = shdr[index_sec_entry].sh_addr;
    		phdr->p_paddr = shdr[index_sec_entry].sh_addr;
    		phdr->p_memsz = phdr->p_filesz;
    		if (index_sec_text != -1 && index_sec_data != -1 && shdr[index_sec_data].sh_size != 0)
    			phdr->p_flags = 0x1 + 0x2 + 0x4; // Execute + Write + Read
    		else if (index_sec_text != -1)
			phdr->p_flags = 0x1 + 0x4; // Execute + Read
		else
			phdr->p_flags = 0x2 + 0x4; // Write + Read
    		/* Trouver le PGCD de p_vaddr et p_offset pour assurer que */
    		/* p_addr % p_align == p_offset % p_align */
    		phdr->p_align = PGCD(phdr->p_vaddr, phdr->p_offset);
    	}/* '.text' et '.data' ne sont pas collées */
    	else{
    		/* Créer 2 en-têtes de programme, une pour '.text' et l'autre pour '.data' */
    		ehdr->e_phnum = 2;
    		
    		phdr = (Elf32_Phdr *)malloc(sizeof(Elf32_Phdr)*ehdr->e_phnum);
    		assert(phdr != NULL);
    		
    		offset_decal = ehdr->e_phnum * 32;
    		
    		init_phdr(phdr[0], shdr, offset_decal, index_sec_text, 1);
    		init_phdr(phdr[1], shdr, offset_decal, index_sec_data, 0);
    	}
    	
    	/** Décalage **/
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
	fclose(fp_temp);
}
