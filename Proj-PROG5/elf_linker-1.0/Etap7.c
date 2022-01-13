#include "Etap7.h"
#include "read_data_auxiliaries.h"

void corrige_ndx(FILE *fp, int index_supprime, int flag) {
	int count = 0, index_sec_symtab = -1, nb_sym_tab_entry = -1, index = 0;
	int a = 0; // Error flag
	
	Elf32_Ehdr *ehdr = read_header_ELF(fp, flag);
    
    	count = ehdr->e_shnum;    

    	Elf32_Shdr *shdr = read_Section_header(fp, ehdr, flag);
    
	char *shstrtab = read_sh_str_tab(fp, shdr, ehdr->e_shstrndx);
	
	index_sec_symtab = chercher_index_de_section(shdr, shstrtab, count, ".symtab");
	assert(index_sec_symtab >= 0);

	nb_sym_tab_entry = calcul_nombre_entrees(shdr, index_sec_symtab);
	assert(nb_sym_tab_entry >= 0);
	
	Elf32_Sym *temp = read_sym_tab(fp, shdr, index_sec_symtab, nb_sym_tab_entry, flag);
	
	Elf32_Sym *symdr = (Elf32_Sym *)malloc(sizeof(Elf32_Sym)*nb_sym_tab_entry);
	assert(symdr != NULL);
	a = fseek(fp, shdr[index_sec_symtab].sh_offset, SEEK_SET);
	assert(a == 0);
	
	for (int j = 0; j < nb_sym_tab_entry; j++) {
		// Passer les symboles ayant une indice étant déjà supprimée
		if (temp[j].st_shndx != index_supprime) {
			fread(&symdr[index], sizeof(Elf32_Sym), 1, fp);
			inverse_data(&symdr[index], sizeof(Elf32_Sym), "Symbol");
			if ((symdr[index].st_shndx > index_supprime) && (symdr[index].st_shndx < 0xfff1))
				symdr[index].st_shndx = symdr[index].st_shndx - 1;
			index++;
		}
	}
	
	/* Ré-écrit la table de symboles */
	a = fseek(fp, shdr[index_sec_symtab].sh_offset, SEEK_SET);
	assert(a == 0);
	for (int i = 0; i < nb_sym_tab_entry; i++) 
		my_write(&symdr[i], sizeof(Elf32_Sym), 1, fp, "Symbol", flag);
	
	free(temp);
	free(symdr);
	free(ehdr);
	free(shdr);
}

int addr_charge(FILE *fp, char *sec_charge, uint32_t addr, int flag) {
	int count = 0, index_sec_charge = -1, index_sec_symtab = -1, nb_sym_tab_entry = 0;
	int a = 0; // Error flag

	Elf32_Ehdr *ehdr = read_header_ELF(fp, flag);
    
    	count = ehdr->e_shnum;    

    	Elf32_Shdr *shdr = read_Section_header(fp, ehdr, flag);
    
	char *shstrtab = read_sh_str_tab(fp, shdr, ehdr->e_shstrndx);
	
	index_sec_symtab = chercher_index_de_section(shdr, shstrtab, count, ".symtab");
	assert(index_sec_symtab >= 0);
	index_sec_charge = chercher_index_de_section(shdr, shstrtab, count, sec_charge);
	if (index_sec_charge < 0) {
		printf("Il n'y a pas de section %s.\n", sec_charge);
		return 0;
	}
	
	printf("-Commencer à charger la section %s.\n", sec_charge);
	
	nb_sym_tab_entry = calcul_nombre_entrees(shdr, index_sec_symtab);
	Elf32_Sym *symdr = read_sym_tab(fp, shdr, index_sec_symtab, nb_sym_tab_entry, flag);
	
	/* Corriger la valeur de chaque symbole par l'addresse chargée */
	a = fseek(fp, shdr[index_sec_symtab].sh_offset, SEEK_SET);
	assert(a == 0);
	for (int j = 0; j < nb_sym_tab_entry; j++) {
		if (symdr[j].st_shndx == index_sec_charge) 
			symdr[j].st_value = symdr[j].st_value + addr;
		my_write(&symdr[j], sizeof(Elf32_Sym), 1, fp, "Symbol", flag);
	}
	
	/* Corriger l'addresse de la section chargée */
	shdr[index_sec_charge].sh_addr = addr;
		  		
	/* Ré-écrit la table de sections */
	a = fseek(fp, ehdr->e_shoff, SEEK_SET);
	assert(a == 0);
	for (int n = 0; n < count; n++)
		my_write(&shdr[n], sizeof(Elf32_Shdr), 1, fp, "shdr", flag);

	free(symdr);
	free(ehdr);
	free(shdr);
	
	return index_sec_charge;
}
