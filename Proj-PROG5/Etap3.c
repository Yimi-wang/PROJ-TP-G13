#include "Etap3.h"

void affichage_contenu_section(int numero, char* temp, Elf32_Shdr *shdr, FILE * fp) {
	int a = 0;
	uint8_t *data_section = (uint8_t *)malloc(sizeof(uint8_t)*shdr[numero].sh_size);
	assert(data_section != NULL);
	
	printf("Nom de cette section: %s\n", temp); 
	printf("Offset de cette section: %x\n", shdr[numero].sh_offset);
	printf("Taille de cette section: %x\n", shdr[numero].sh_size);
	
	a = fseek(fp, shdr[numero].sh_offset, SEEK_SET);
	assert(a == 0);
	a = fread(data_section, sizeof(uint8_t)*shdr[numero].sh_size, 1, fp);
	assert(a != 0);
	
	for (int j = 0; j < shdr[numero].sh_size; j++) 
		printf("%x", data_section[j]);
		
	free(data_section);
}

void etap3(Elf32_Ehdr* ehdr, FILE * fp){
	int a = 0; // Error flag
	int numero = -1, count = 0; 
  	
  	a = fseek(fp, 0, SEEK_SET);
  	assert(a == 0);
	a = fread(ehdr, sizeof(Elf32_Ehdr), 1, fp);   
  	assert(a != 0);
	
	count = ehdr->e_shnum; 

	Elf32_Shdr *shdr = (Elf32_Shdr*)malloc(sizeof(Elf32_Shdr) * count);
	assert(shdr != NULL);

	a = fseek(fp, ehdr->e_shoff, SEEK_SET);
	assert(a == 0);
	a = fread(shdr, sizeof(Elf32_Shdr), count, fp);
	assert(a != 0);
	
	// e_shstrndx est l'index de l’entrée correspondant à la table des chaînes
	char shstrtab[shdr[ehdr->e_shstrndx].sh_size];
	
	a = fseek(fp, shdr[ehdr->e_shstrndx].sh_offset, SEEK_SET);
	assert(a == 0);
	a = fread(shstrtab, shdr[ehdr->e_shstrndx].sh_size, 1, fp);
	assert(a != 0);

	char *temp = shstrtab;
	
	char str[20];
	printf("\ndonner le nom de section: ");
	scanf("%20s", str);
	
	numero = atoi(str);
	if (str[0] != 0 && numero == 0)
		numero = -1;
	
	if (numero > -1) {
		temp += shdr[numero].sh_name;
		affichage_contenu_section(numero, temp, shdr, fp);
	}else{
		for (int i = 0; i < count; i++) {
			temp = shstrtab;
			temp = temp + shdr[i].sh_name;
			if (strcmp(temp, str) == 0) {
				affichage_contenu_section(i, temp, shdr, fp);
				break;
			}
		}
	}
  	printf("\n");
  	free(shdr);
}
