#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>
#include "Etap1.h"
#include "Etap2.h"
#include "Etap3.h"
#include "Etap4.h"
#include "Etap5.h"

int main(int argc, char *argv[])
{
  if (argc != 3){
  	printf("Erreur, le format correct: ./Pharse1 nom_fichier 1/2/3...\n");
  	exit(0);
  }
  
  int n = atoi(argv[2]);
  FILE *fp;
  char str[20];
  Elf32_Ehdr *ehdr = (Elf32_Ehdr *)malloc(sizeof(Elf32_Ehdr));
  
  fp = fopen(argv[1], "rb");
  if(fp == NULL) {
    printf("Erreur d'ouverture du fichier: %s\n", argv[1]);
    exit(1);
  }

  fread(str, 1, 5, fp);
  if(str[0] != 0x7f || str[1] != 'E' || str[2] != 'L' || str[3] != 'F') {
    printf("%s n'est pas un fichier ELF.\n", argv[1]);
    exit(1);
  }  
  
  switch (n){
    case 1:
    	printf("Affichage de l'en-tête (header ELF) de '%s':\n", argv[1]);
    	etap1(ehdr,fp);
    	break;
    
    case 2:
    	printf("Affichage de la table des sections (Section header table) de '%s':\n", argv[1]);
    	etap2(ehdr,fp);
    	break;
    
    case 3:
    	printf("Affichage du contenu d'une section du fichier %s: ", argv[1]);
    	etap3(ehdr,fp);
    	break;
    
    case 4:
    	printf("Affichage de la table des symboles du fichier %s: ", argv[1]);
    	etap4(ehdr,fp);
    	break;
    
    case 5:
    	printf("Affichage des tables de réimplantation du fichier %s: \n", argv[1]);
    	etap5(ehdr,fp);
    	break;
    
    default:
    	printf("Ce programme ne serve que pour les étapes 1 ~ 5.\n");
    	break;
  }
  
  fclose(fp);
  return 0;
}
