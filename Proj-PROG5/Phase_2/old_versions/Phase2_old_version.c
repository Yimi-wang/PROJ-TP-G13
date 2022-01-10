#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>
#include "Etap6.h"
#include "Etap7.h"
#include "Etap8_9.h"

static char* new_name1(char *original_name) {
	char *delim = ".";
	char *new = (char *)malloc(sizeof(original_name)+sizeof(char)*8);
	strcpy(new, strtok(original_name, delim));
	strcat(new, "_sortie");   
	
	return new;
}

int main(int argc, char *argv[])
{
  if (argc != 2){
  	printf("Erreur, le format correct: ./Phase2 nom_fichier\n");
  	exit(0);
  }
  
  int index_modif1 = 0, index_modif2 = 0;
  unsigned int a = 0;
  uint8_t *bintemp = NULL;
  FILE *fp, *fp_sortie;
  char str[20], str_section[20];
  char *newfilename;
  int flag = 0;
  
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
  
  newfilename = new_name1(argv[1]);
  fp_sortie = fopen(newfilename, "w+"); 
  
  bintemp = (uint8_t *)malloc(sizeof(uint8_t)*1024);
  
  /* Copier les données de la source à la sortie */
  fseek(fp, 0, SEEK_SET);
  while(!feof(fp)) {
  	a = fread(bintemp, 1, 1024, fp);
  	assert(a != 0);
  	if (a != 1024) 
  		a = fwrite(bintemp, a, 1, fp_sortie);
  	else
  		a = fwrite(bintemp, 1024, 1, fp_sortie);
  	assert(a >= 1);
  }
  
  flag = 1;
  if (flag == 0) {
	printf("Quelle section sera modifiée? : ");
	scanf("%20s", str_section);
	index_modif1 = etap6(fp_sortie, str_section);
	corrige_ndx(fp_sortie, index_modif1);
	addr_charge(fp_sortie, ".text", 0x40);
  }else{
  	index_modif1 = addr_charge(fp_sortie, ".text", 0x58);
  	index_modif2 = addr_charge(fp_sortie, ".data", 0x1000);
  	printf(".text: %d, .data: %d\n", index_modif1, index_modif2);
  	proc_reimplants(fp_sortie);
  }
  
  fclose(fp);
  fclose(fp_sortie);
  return 0;
}