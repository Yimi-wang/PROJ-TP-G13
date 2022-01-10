#include "Etap7.h"
#include "read_data_auxiliaries.h"

void corrige_ndx(FILE *fp, int index_supprime) {
	int count = 0, index_sec_symtab = -1, nb_sym_tab_entry = -1, index = 0;
	int a = 0; // Error flag
	
	Elf32_Ehdr *ehdr = read_header_ELF(fp);

	count = ehdr->e_shnum;
	
	Elf32_Shdr *shdr = read_Section_header(fp, ehdr);
    
	char shstrtab[shdr[ehdr->e_shstrndx].sh_size];
	a = fseek(fp, shdr[ehdr->e_shstrndx].sh_offset, SEEK_SET);
	assert(a == 0);
	a = fread(shstrtab, shdr[ehdr->e_shstrndx].sh_size, 1, fp);
	assert(a != 0);
	
	index_sec_symtab = chercher_index_de_section(shdr, shstrtab, count, ".symtab");
	assert(index_sec_symtab >= 0);

	nb_sym_tab_entry = calcul_nombre_entrees(shdr, index_sec_symtab);
	assert(nb_sym_tab_entry >= 0);
	
	Elf32_Sym *temp = read_sym_tab(fp, shdr, index_sec_symtab, nb_sym_tab_entry);
	
	Elf32_Sym *symdr = (Elf32_Sym *)malloc(sizeof(Elf32_Sym)*nb_sym_tab_entry);
	assert(symdr != NULL);
	a = fseek(fp, shdr[index_sec_symtab].sh_offset, SEEK_SET);
	assert(a == 0);
	
	for (int j = 0; j < nb_sym_tab_entry; j++) {
		// Passer les symboles ayant une indice étant déjà supprimée
		if (temp[j].st_shndx != index_supprime) {
			fread(&symdr[index], sizeof(Elf32_Sym), 1, fp);
			if ((temp[j].st_shndx > index_supprime) && (temp[j].st_shndx < 0xfff1))
				symdr[index].st_shndx = symdr[index].st_shndx - 1;
			index++;
		}
	}
	
	/* Ré-écrit la table de symboles */
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
	int count = 0, index_sec_charge = -1, index_sec_symtab = -1, nb_sym_tab_entry = 0;
	int a = 0; // Error flag
  	
	Elf32_Ehdr *ehdr = read_header_ELF(fp);

	count = ehdr->e_shnum;
	
	Elf32_Shdr *shdr = read_Section_header(fp, ehdr);
    
	char shstrtab[shdr[ehdr->e_shstrndx].sh_size];
	a = fseek(fp, shdr[ehdr->e_shstrndx].sh_offset, SEEK_SET);
	assert(a == 0);
	a = fread(shstrtab, shdr[ehdr->e_shstrndx].sh_size, 1, fp);
	assert(a != 0);
	
	index_sec_symtab = chercher_index_de_section(shdr, shstrtab, count, ".symtab");
	assert(index_sec_symtab >= 0);
	index_sec_charge = chercher_index_de_section(shdr, shstrtab, count, sec_charge);
	assert(index_sec_charge >= 0);
	
	nb_sym_tab_entry = calcul_nombre_entrees(shdr, index_sec_symtab);
	Elf32_Sym *symdr = read_sym_tab(fp, shdr, index_sec_symtab, nb_sym_tab_entry);
	
	/* Corriger la valeur de chaque symbole par l'addresse chargée */
	for (int j = 0; j < nb_sym_tab_entry; j++) {
		if (symdr[j].st_shndx == index_sec_charge) 
			symdr[j].st_value = symdr[j].st_value + addr;
	}
	
	/* Corriger l'addresse de la section chargée */
	shdr[index_sec_charge].sh_addr = addr;
	
	/* Ré-écrit la table de symboles */
	a = fseek(fp, shdr[index_sec_symtab].sh_offset, SEEK_SET);
	assert(a == 0);
	a = fwrite(symdr, shdr[index_sec_symtab].sh_size, 1, fp);
	assert(a >= 1);
	
	/* Ré-écrit la table de sections */
	a = fseek(fp, ehdr->e_shoff, SEEK_SET);
	assert(a == 0);
	a = fwrite(shdr, (ehdr->e_shnum * ehdr->e_shentsize), 1, fp);
	assert(a >= 1);
	
	free(symdr);
	free(ehdr);
	free(shdr);
	
	return index_sec_charge;
}
