#include "Etap8_9.h"
#include "read_data_auxiliaries.h"

static char *nom_section_correspondant(char *source) {
	char *s = (char *)malloc(sizeof(source));
	char *delim = ".";
	char *new = (char *)malloc(sizeof(s)-sizeof(char)*3+sizeof(char)*1);

	strcpy(s, source);
	strcpy(new, ".");
	strtok(s, delim);
	strcat(new, strtok(NULL, delim));
	
	return new;
}

void proc_reimplants(FILE *fp, int flag) {
	char *nom_rel = NULL, *nom_sec = NULL;
	Elf32_Rel *reldr = NULL;
	int count = 0, index_sec_cor = -1, index_sec_symtab = -1;
	uint32_t new_addr = 0, ndx_rel_sym = 0;
	uint32_t S = 0, A = 0, P = 0, T = 0;
	uint8_t *data_sec = NULL;
	unsigned char rel_type = 0, sym_type = 0;
	int nb_entry_sym_tab = 0, nb_entry_rel = 0;
  	
	Elf32_Ehdr *ehdr = read_header_ELF(fp, flag);
    
    	count = ehdr->e_shnum;    

    	Elf32_Shdr *shdr = read_Section_header(fp, ehdr, flag);
    
	char *shstrtab = read_sh_str_tab(fp, shdr, ehdr->e_shstrndx);
	
	index_sec_symtab = chercher_index_de_section(shdr, shstrtab, count, ".symtab");
	
	/* Lire les données de '.symtab' */
	nb_entry_sym_tab = calcul_nombre_entrees(shdr, index_sec_symtab);
	Elf32_Sym *sym = read_sym_tab(fp, shdr, index_sec_symtab, nb_entry_sym_tab, flag); 
		
	/* Réimplantation */
	for (int i = 0; i < count; i++) {
		nom_rel = shstrtab;
		nom_rel = nom_rel + shdr[i].sh_name;
		if (strcmp(nom_rel, ".rel.text") != 0 && strcmp(nom_rel, ".rel.data") != 0)
			continue;
		// Négliger le type ".rela"
		if (shdr[i].sh_type == 9) {
			/*nom_rel = shstrtab;
			nom_rel = nom_rel + shdr[i].sh_name;*/
			nom_sec = nom_section_correspondant(nom_rel);
			index_sec_cor = chercher_index_de_section(shdr, shstrtab, count, nom_sec);
			assert(index_sec_cor >= 0);
			
			/* Obetenir les données de '.rel.*' */
			nb_entry_rel = calcul_nombre_entrees(shdr, i);
			reldr = read_rel_tab(fp, shdr, i, nb_entry_rel, flag);
			
			/* Prendre la section correspondante */
			data_sec = read_section(fp, shdr, index_sec_cor, flag);
			printf("-Commencer à faire la réimplantations pour la section : ");
			printf("%s\n", nom_sec);
			for (int j = 0; j < nb_entry_rel; j++) {	
				ndx_rel_sym = (reldr[j].r_info >> 8);
				rel_type = (unsigned char) reldr[j].r_info;
				sym_type = sym[ndx_rel_sym].st_info & 0xf;
				
				S = sym[ndx_rel_sym].st_value;
				A = 0; // A = 0 toujours
				T = 1;
				P = shdr[index_sec_cor].sh_addr + reldr[j].r_offset;

				// Type R_ARM_ABS*
				if ((rel_type == 2) || (rel_type == 5) || (rel_type == 8)) {
					if ((rel_type == 2) && (sym_type == 2)) // STT_FUNC == 2
						new_addr = T;
					else
						new_addr = S + A;
				}// Type R_ARM_JUMP24 & R_ARM_CALL
				else if ((rel_type == 28) || (rel_type == 29)) {
					if (sym_type == 2)
						new_addr = 1 - P;
					else
						new_addr = S + A - P;
					// Result Mask
					new_addr = new_addr & 0x03FFFFFE;
					new_addr = new_addr >> 2;
				}

				uint32_t value = 0;
				value |= (data_sec[reldr[j].r_offset + 0] & 0xff);
				value |= ((data_sec[reldr[j].r_offset + 1] & 0xff) << 8);
				value |= ((data_sec[reldr[j].r_offset + 2] & 0xff) << 16);
				value |= 0xff000000;

				data_sec[reldr[j].r_offset + 0] = (new_addr + value) & 0x000000ff;
				data_sec[reldr[j].r_offset + 1] = ((new_addr + value) & 0x0000ff00) >> 8;
				data_sec[reldr[j].r_offset + 2] = ((new_addr + value) & 0x00ff0000) >> 16;

				/* Ne bouger pas l'octet pour indiquer le type d'instruction */
				data_sec[reldr[j].r_offset + 3] |= (new_addr & 0xff000000) >> 24;
				
			}
			
			/* Ré-écrit la section (.text) changée à fichier */
			fseek(fp, shdr[index_sec_cor].sh_offset, SEEK_SET);
			my_write(data_sec, shdr[index_sec_cor].sh_size, 1, fp, "normal", flag);
		} 
	}
	
	free(reldr);
	free(data_sec);
	free(sym);
	free(ehdr);
	free(shdr);
}	
