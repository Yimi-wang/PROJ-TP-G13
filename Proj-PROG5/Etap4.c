#include "Etap4.h"

void etap4(Elf32_Ehdr* ehdr, FILE * fp) {
    int a = 0; // Error flag
    int count = 0, index_SymTab = 0, index_StrTab = 0, nb_entries_SymTab = 0;
    char *temp = NULL;
    char *str = ".symtab";
    char *str2 = ".strtab";
    
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
      if (strcmp(temp, str) == 0) 
      	index_SymTab = i;
      else if (strcmp(temp, str2) == 0)
      	index_StrTab = i;
    }

    char strtab[shdr[index_StrTab].sh_size];
    a = fseek(fp, shdr[index_StrTab].sh_offset, SEEK_SET);
    assert(a == 0);
    a = fread(strtab, shdr[index_StrTab].sh_size, 1, fp);
    assert(a != 0);

    nb_entries_SymTab = shdr[index_SymTab].sh_size / shdr[index_SymTab].sh_entsize;
    printf("\nSymbol table '.symtab' contains %d entries :", nb_entries_SymTab);

    Elf32_Sym *sym = (Elf32_Sym *)malloc(sizeof(Elf32_Sym)*nb_entries_SymTab);
    assert(sym != NULL);
    a = fseek(fp, shdr[index_SymTab].sh_offset, SEEK_SET);
    assert(a == 0);
    a = fread(sym, sizeof(Elf32_Sym), nb_entries_SymTab, fp);
    assert(a != 0);
    
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
