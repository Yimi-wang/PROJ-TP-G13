#include "Etap5.h"
#include "read_data_auxiliaries.h"

static void type_reimplantation(unsigned char index, int flag_ARM) {
	if (flag_ARM == 1) {
		switch(index) {
			case 0 : printf("R_ARM_NONE\t"); break;
			case 1 : printf("R_ARM_PC24\t"); break;
			case 2 : printf("R_ARM_ABS32\t"); break;
			case 3 : printf("R_ARM_REL32\t"); break;
			case 4 : printf("R_ARM_LDR_PC_G0\t"); break;
			case 5 : printf("R_ARM_ABS16\t"); break;
			case 6 : printf("R_ARM_ABS12\t"); break;
			case 7 : printf("R_ARM_THM_ABS5\t"); break;
			case 8 : printf("R_ARM_ABS8\t"); break;
			case 9 : printf("R_ARM_SBREL32\t"); break;
			case 10 : printf("R_ARM_THM_CALL\t"); break;
			case 11 : printf("R_ARM_THM_PC8\t"); break;
			case 12 : printf("R_ARM_BREL_ADJ\t"); break;
			case 13 : printf("R_ARM_TLS_DESC\t"); break;
			case 17 : printf("R_ARM_TLS_DTPMOD32\t"); break;
			case 18 : printf("R_ARM_TLS_DTPOFF32\t"); break;
			case 19 : printf("R_ARM_TLS_TPOFF32\t"); break;
			case 21 : printf("R_ARM_GLOB_DAT\t"); break;
			case 22 : printf("R_ARM_JUMP_SLOT\t"); break;
			case 23 : printf("R_ARM_RELATIVE\t"); break;
			case 24 : printf("R_ARM_GOTOFF32\t"); break;
			case 25 : printf("R_ARM_BASE_PREL\t"); break;
			case 26 : printf("R_ARM_GOT_BREL\t"); break;
			case 27 : printf("R_ARM_PLT32\t"); break;
			case 28 : printf("R_ARM_CALL\t"); break;
			case 29 : printf("R_ARM_JUMP24\t"); break;
			case 30 : printf("R_ARM_THM_JUMP24\t"); break;
			default : printf("%d\t", index); break;
		}
	}else{
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
}

void etap5(Elf32_Ehdr* ehdr, FILE * fp, int flag) {
	char *temp = NULL;
	int count = 0, nb_entries_rel_a = 0;
	int a = 0; // Error flag
	
	ehdr = read_header_ELF(fp, flag);
    
    	count = ehdr->e_shnum;    

    	Elf32_Shdr *shdr = read_Section_header(fp, ehdr, flag);

    	char *shstrtab = read_sh_str_tab(fp, shdr, ehdr->e_shstrndx);

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
				Elf32_Rela *rela = read_rela_tab(fp, shdr, i, nb_entries_rel_a, flag);
				for (int j = 0; j < nb_entries_rel_a; j++) {
					printf("%012x\t", rela[j].r_offset);
					if (ehdr->e_machine == 40) // si la machine cible est ARM
						type_reimplantation((unsigned char) rela[j].r_info, 1);
					else
						type_reimplantation((unsigned char) rela[j].r_info, 0);
					printf("%u\t", (rela[j].r_info >> 8));
					printf("\n");
				}
				free(rela);
			}else{ // .rel.*
				Elf32_Rel *rel = read_rel_tab(fp, shdr, i, nb_entries_rel_a, flag);
				for (int j = 0; j < nb_entries_rel_a; j++) {
					printf("%012x\t", rel[j].r_offset);
					if (ehdr->e_machine == 40) // si la machine cible est ARM
						type_reimplantation((unsigned char) rel[j].r_info, 1);
					else
						type_reimplantation((unsigned char) rel[j].r_info, 0);
					printf("%u\t", (rel[j].r_info >> 8));
					printf("\n");
				}
				free(rel);
			}
		}
	}
	free(shdr);
}

