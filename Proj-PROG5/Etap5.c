#include "Etap5.h"

static void type_reimplantation(unsigned char index) {
	switch(index) {
		case 0 : printf("R_386_NONE\t"); break;
		case 1 : printf("R_386_64\t"); break;
		case 2 : printf("R_386_PC32\t"); break;
		case 3 : printf("R_386_GOT32\t"); break;
		case 4 : printf("R_386_PLT32\t"); break;
		case 5 : printf("R_386_COPY\t"); break;
		case 6 : printf("R_386_GLOB_DAT\t"); break;
		case 7 : printf("R_386_JUMP_SLOT\t"); break;
		case 8 : printf("R_386_RELATIVE\t"); break;
		case 9 : printf("R_386_GOTOFF\t"); break;
		case 10 : printf("R_386_GOTPC\t"); break;
		case 11 : printf("R_386_32S\t"); break;
		case 12 : printf("R_386_16\t"); break;
		case 13 : printf("R_386_PC16\t"); break;
		case 14 : printf("R_386_8\t"); break;
		case 15 : printf("R_386_PC8\t"); break;
		case 24 : printf("R_386_PC64\t"); break;
		case 25 : printf("R_386_GOTOFF64\t"); break;
		case 26 : printf("R_386_GOTPC32\t"); break;
		case 32 : printf("R_386_SIZE32\t"); break;
		case 33 : printf("R_386_SIZE64\t"); break;
		case 43 : printf("R_386_GOT32X\t"); break;
		default : printf("%d\t", index); break;
	}

}

void etap5(Elf32_Ehdr* ehdr, FILE * fp) {
	char *temp = NULL;
	int count = 0, nb_entries_rel_a = 0;
	int a = 0; // Error flag
	
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
		if (shdr[i].sh_type == 4 || shdr[i].sh_type == 9) { 
			temp = shstrtab;
			temp = temp + shdr[i].sh_name;
      			assert(shdr[i].sh_entsize != 0);
			nb_entries_rel_a = shdr[i].sh_size / shdr[i].sh_entsize;
			
			printf("Relocation section '%s' ", temp);
			printf("at offset 0x%x ", shdr[i].sh_offset);
			if (nb_entries_rel_a == 1)
				printf("contains 1 entry:\n");
			else 
				printf("contains %d entries:\n", nb_entries_rel_a);
			
			printf("  offset\t");
			printf("Type\t");
			printf("\tNdx_Sym\n");
			
			a = fseek(fp, shdr[i].sh_offset, SEEK_SET);
      			assert(a == 0);

			if (shdr[i].sh_type == 4) { // .rela.*
				Elf32_Rela *rela = (Elf32_Rela *)malloc(sizeof(Elf32_Rela)*nb_entries_rel_a);
        			assert(rela != NULL);
				a = fread(rela, sizeof(Elf32_Rela), nb_entries_rel_a, fp);
        			assert(a != 0);
				for (int j = 0; j < nb_entries_rel_a; j++) {
					printf("%012x\t", rela[j].r_offset);
					//type_reimplantation(rela[j].r_info & 0xff); // %ld
					type_reimplantation((unsigned char) rela[j].r_info);
					printf("%u\t", (rela[j].r_info >> 8));
					printf("\n");
				}
				free(rela);
			}else{ // .rel.*
				Elf32_Rel *rel = (Elf32_Rel *)malloc(sizeof(Elf32_Rel)*nb_entries_rel_a);
        			assert(rel != NULL);
				a = fread(rel, sizeof(Elf32_Rel), nb_entries_rel_a, fp);
        			assert(a != 0);
				for (int j = 0; j < nb_entries_rel_a; j++) {
					printf("%012x\t", rel[j].r_offset);
					//type_reimplantation(rel[j].r_info & 0xff); // %ld
					type_reimplantation((unsigned char) rel[j].r_info);
					printf("%u\t", (rel[j].r_info >> 8));
					printf("\n");
				}
				free(rel);
			}
		}
	}
	free(shdr);
}

