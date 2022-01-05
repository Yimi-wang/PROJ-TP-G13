#include "Etap1.h"

void etap1(Elf32_Ehdr* ehdr, FILE * fp){
  fseek(fp, 0, SEEK_SET);
  fread(ehdr, sizeof(Elf32_Ehdr), 1, fp);

  printf("la plateforme cible:\t\t");
  switch(ehdr->e_machine) {
        case 0 : printf("No machine\n"); break;
        case 1 : printf("AT&T WE 32100\n"); break;
        case 2 : printf("SPARC\n"); break;
        case 3 : printf("Intel Architecture\n"); break;
        case 4 : printf("Motorola 68000\n"); break;
        case 5 : printf("Motorola 88000\n"); break;
        case 7 : printf("Intel 80860\n"); break;
        case 8 : printf("MIPS RS3000 Big-Endian\n"); break;
        case 10 : printf("MIPS RS4000 Big-Endian\n"); break;
        case 40 : printf("ARM\n"); break;
        case 62 : printf("Advanced Micro Devices X86-64\n"); break;
  }

  printf("la taille des mots:\t\t");
  switch(ehdr->e_ident[4]){ //4 == EI_CLASS
    case 0 : printf("Invalid class\n"); break;
    case 1 : printf("32-bit objects\n"); break;
    case 2 : printf("64-bit objects\n"); break;
  }

  printf("le type de fichier ELF:\t\t");
  switch(ehdr->e_type){
    case 0 : printf("NONE (No file type)\n"); break;
    case 1 : printf("REL (Relocatable file)\n"); break;
    case 2 : printf("EXEC (Executable file)\n"); break;
    case 3 : printf("DYN (Shared object file)\n"); break;
    case 4 : printf("CORE (Core file)\n"); break;
    case 0xff00: printf("LOPROC (Processor-specific)\n"); break;
    case 0xffff: printf("HIPROC (Processor-specific)\n"); break;
  }
  
  printf("%d\n", ehdr->e_phoff);

  printf("la specication de la table des sections :\n");
  printf("--position dans le fichier (offset): %d octets\n", ehdr->e_shoff);
  printf("--taille globale de la table: %d\n", (ehdr->e_shnum*ehdr->e_shentsize));
  printf("--nombre des sections (entrées): %d\n", ehdr->e_shnum);
  
  printf("l'index de l'entree correspondant à la table des chaines: %u\n", ehdr->e_shstrndx);
  
  printf("la taille de l'en-tete: %u octets\n", ehdr->e_ehsize);
}
