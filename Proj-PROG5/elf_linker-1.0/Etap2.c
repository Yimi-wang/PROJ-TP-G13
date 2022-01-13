#include "Etap2.h"
#include "read_data_auxiliaries.h"

void etap2(Elf32_Ehdr* ehdr, FILE * fp, int flag){
    ehdr = read_header_ELF(fp, flag);
    
    int count = ehdr->e_shnum;    

    Elf32_Shdr *shdr = read_Section_header(fp, ehdr, flag);
    
    char *strtable = read_sh_str_tab(fp, shdr, ehdr->e_shstrndx);
    
    printf("[numero]\t");
    printf("nom\t\t\t");
    printf("taille\t\t");
    printf("type\t\t");
    printf("attributs(flags) ");
    printf("offset");
    printf("\n");
    
    for(int i = 0; i < count; i++) { 
        //le numero de section
        printf("[%02d]\t", i);
        
        //le nom de section
        printf("%s",  &strtable[shdr[i].sh_name]);
        for(int j = 0; j < 30 - strlen(&strtable[shdr[i].sh_name]); ++j) {
            putchar(' ');
        }
        //taille
        printf("\t%u bytes\t", shdr[i].sh_size);
        if(shdr[i].sh_size<10)
          printf("\t");
        
        //type
        uint8_t flag = 0;
        switch(shdr[i].sh_type) {               
            case 0 : printf("NULL\t\t"); break;
            case 1 : printf("PROGBITS\t"); break;
            case 2 : printf("SYMTAB\t\t"); break;
            case 3 : printf("STRTAB\t\t"); break;
            case 4 : printf("RELA\t\t"); break;
            case 5 : printf("HASH\t"); break;
            case 6 : printf("DYNAMIC\t"); break;
            case 7 : printf("NOTE\t\t"); break;
            case 8 : printf("NOBITS\t\t"); break;
            case 9 : printf("REL\t\t"); break;
            case 10 : printf("SHLIB\t"); break;
            case 11 : printf("DYNSYM\t"); break;
            case 14 : printf("INIT_ARRAY\t"); break;
            case 15 : printf("FINI_ARRAY\t"); break;
            case 0x70000000 : printf("LOPROC\t"); break;
            case 0x7fffffff : printf("HIPROC\t"); break;
            case 0x80000000 : printf("LOUSER\t"); break;
            case 0xffffffff : printf("HIUSER\t"); break;
            case 0x6ffffff6 : printf("GNU_HASH\t"); break;
            case 0x6fffffff : printf("GNU_versym\t"); break;
            case 0x6ffffffe : printf("GNU_verneed\t"); break;
            //ELF for ARM sections types
            case 0x70000001 : printf("ARM_EXIDX\t");break;
            case 0x70000002 : printf("ARM_PREEMPTMAP\t");break;
            case 0x70000003 : printf("ARM_ATTRIBUTES \t");break;
            case 0x70000004 : printf("ARM_DEBUGOVERLAY \t");break;
            case 0x70000005 : printf("ARM_OVERLAYSECTION \t");break;
            default :	fseek(fp, shdr[i].sh_offset, SEEK_SET);
            		fread(&flag, 1, 1, fp);
            		if (flag == 1)
            			printf("GROUP\t\t");
            		else
            			printf("\t%u\t",shdr[i].sh_type);
            		break; 
        }

        printf("0x%06x\t ", shdr[i].sh_flags); //%lu -> décimal

        printf("0x%06x\n", shdr[i].sh_offset);
    }
    free(shdr);
}

