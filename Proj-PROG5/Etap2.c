#include "Etap2.h"

void etap2(Elf32_Ehdr* ehdr, FILE * fp){
    char strtable[9999];
    int a = 0; //Error flag
    
    a = fseek(fp, 0, SEEK_SET);
    assert(a == 0);
    
    a = fread(ehdr, sizeof(Elf32_Ehdr), 1, fp);
    assert(a != 0);
    
    int count = ehdr->e_shnum;    

    Elf32_Shdr *shdr = (Elf32_Shdr*)malloc(sizeof(Elf32_Shdr) * count);
    assert(shdr != NULL);
	
    a = fseek(fp, ehdr->e_shoff, SEEK_SET);
    assert(a == 0);
    a = fread(shdr, sizeof(Elf32_Shdr), count, fp);
    assert(a != 0);
    
    a = fseek(fp, shdr[ehdr->e_shstrndx].sh_offset, SEEK_SET);
    assert(a == 0);
    a = fread(strtable, shdr[ehdr->e_shstrndx].sh_size, 1, fp);
    assert(a != 0);
    
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

