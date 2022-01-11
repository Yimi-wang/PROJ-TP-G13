#include "Etap4.h"
#include "read_data_auxiliaries.h"

void etap4(Elf32_Ehdr* ehdr, FILE * fp, int flag) {
    int a = 0; // Error flag
    int count = 0, index_SymTab = -1, index_StrTab = -1, nb_entries_SymTab = 0;
    char *temp = NULL;
    
    ehdr = read_header_ELF(fp, flag);
    
    count = ehdr->e_shnum;    

    Elf32_Shdr *shdr = read_Section_header(fp, ehdr, flag);

    char *shstrtab = read_sh_str_tab(fp, shdr, ehdr->e_shstrndx);
    
    index_SymTab = chercher_index_de_section(shdr, shstrtab, count, ".symtab");
    assert(index_SymTab != -1);
    index_StrTab = chercher_index_de_section(shdr, shstrtab, count, ".strtab");
    assert(index_StrTab != -1);

    char strtab[shdr[index_StrTab].sh_size];
    a = fseek(fp, shdr[index_StrTab].sh_offset, SEEK_SET);
    assert(a == 0);
    a = fread(strtab, shdr[index_StrTab].sh_size, 1, fp);
    assert(a != 0);
    
    assert(shdr[index_SymTab].sh_entsize != 0);
    nb_entries_SymTab = shdr[index_SymTab].sh_size / shdr[index_SymTab].sh_entsize;
    printf("\nSymbol table '.symtab' contains %d entries :", nb_entries_SymTab);

    Elf32_Sym *sym = read_sym_tab(fp, shdr, index_SymTab, nb_entries_SymTab, flag);
    
    printf("\nValue\t\t");
    printf("Size\t");
    printf("Type\t");
    printf("Bind (Portée)\t");
    printf("Ndx\t");
    printf("Nom");
    printf("\n");
    
    for (int j = 0; j < nb_entries_SymTab; j++) {	
      	printf("%08x\t", sym[j].st_value);

    	printf("%u\t", sym[j].st_size);
    	
    	switch((sym[j].st_info & 0xf)) {
    		case 0 : printf("NOTYPE\t"); break;
    		case 1 : printf("OBJECT\t"); break;
    		case 2 : printf("FUNC\t"); break;
    		case 3 : printf("SECTION\t"); break;
    		case 4 : printf("FILE\t"); break;
    		case 13 : printf("LOPROC\t"); break;
    		case 15 : printf("HIPROC\t"); break;
    	}
    	
    	switch((sym[j].st_info >> 4)) {
    		case 0 : printf("LOCAL\t\t"); break;
    		case 1 : printf("GLOBAL\t\t"); break;
    		case 2 : printf("WEAK\t\t"); break;
    		case 13 : printf("LOPROC\t\t"); break;
    		case 15 : printf("HIPROC\t\t"); break;
    	}
    	
    	switch(sym[j].st_shndx) {
    		case 0xfff1 : printf("ABS\t"); break;
    		case 0xfff2 : printf("COM\t"); break;
    		case 0 : printf("UND\t"); break;
    		default: printf("%hu\t", sym[j].st_shndx); break;
    	}

      	temp = strtab;
    	temp = temp + sym[j].st_name;
    	printf("%s\t", temp);	
    	printf("\n");
    }
}
