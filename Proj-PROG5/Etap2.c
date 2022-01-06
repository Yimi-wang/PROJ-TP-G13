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
    printf("type\t\t\t");
    printf("attributs\t\t");
    printf("offset\t");
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
            case 0 : printf("SHT_NULL\t"); break;
            case 1 : printf("SHT_PROGBITS\t"); break;
            case 2 : printf("SHT_SYMTAB\t"); break;
            case 3 : printf("SHT_STRTAB\t"); break;
            case 4 : printf("SHT_RELA\t"); break;
            case 5 : printf("SHT_HASH\t"); break;
            case 6 : printf("SHT_DYNAMIC\t"); break;
            case 7 : printf("SHT_NOTE\t"); break;
            case 8 : printf("SHT_NOBITS\t"); break;
            case 9 : printf("SHT_REL\t\t"); break;
            case 10 : printf("SHT_SHLIB\t"); break;
            case 11 : printf("SHT_DYNSYM\t"); break;
            case 14 : printf("SHT_INIT_ARRAY\t"); break;
            case 15 : printf("SHT_FINI_ARRAY\t"); break;
            case 17 : printf("SHT_GROUP\t");break;
            case 0x70000000 : printf("SHT_LOPROC\t"); break;
            case 0x7fffffff : printf("SHT_HIPROC\t"); break;
            case 0x80000000 : printf("SHT_LOUSER\t"); break;
            case 0xffffffff : printf("SHT_HIUSER\t"); break;
            case 0x6ffffff6 : printf("SHT_GNU_HASH\t"); break;
            case 0x6fffffff : printf("SHT_GNU_versym\t"); break;
            case 0x6ffffffe : printf("SHT_GNU_verneed\t"); break;
             //ELF for ARM sections types
            case 0x70000001 : printf("SHT_ARM_EXIDX\t");break;
            case 0x70000002 : printf("SHT_ARM_PREEMPTMAP\t");break;
            case 0x70000003 : printf("SHT_ARM_ATTRIBUTES \t");break;
            case 0x70000004 : printf("SHT_ARM_DEBUGOVERLAY \t");break;
            case 0x70000005 : printf("SHT_ARM_OVERLAYSECTION \t");break;
            default :	fseek(fp, shdr[i].sh_offset, SEEK_SET);
            		fread(&flag, 1, 1, fp);
            		if (flag == 1)
            			printf("SHT_GROUP\t");
            		else
            			printf("\t%u\t",shdr[i].sh_type);
            		break; 
        }
        unsigned int flags = shdr[i].sh_flags;
        printf("\t0x%x", flags); //%lu -> décimal

        unsigned int offset = shdr[i].sh_offset;
        printf("\t\t\t0x%x\t", offset);
        
    	printf("\n");    
    }
    free(shdr);
}
