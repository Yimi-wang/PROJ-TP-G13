#include "Etap3.h"
#include "read_data_auxiliaries.h"

void affichage_contenu_section(int numero, char* temp, Elf32_Shdr *shdr, FILE * fp) {
	int a = 0;
	uint8_t *data_section = (uint8_t *)malloc(sizeof(uint8_t)*shdr[numero].sh_size);
	assert(data_section != NULL);
	
	printf("Nom de cette section: %s\n", temp); 
	printf("Offset de cette section: %x\n", shdr[numero].sh_offset);
	printf("Taille de cette section: %x\n", shdr[numero].sh_size);
	printf("Contenu brut de cette section:");
	
	a = fseek(fp, shdr[numero].sh_offset, SEEK_SET);
	assert(a == 0);
	a = fread(data_section, sizeof(uint8_t)*shdr[numero].sh_size, 1, fp);
	assert(a != 0);
	
	for (int j = 0; j < shdr[numero].sh_size; j++) {
		if (j % 16 == 0) {
			printf("\n");
			printf("  0x%08x ", j);
		}	
		else if (j % 4 == 0) 
			printf(" ");
		printf("%02x", data_section[j]);
	}	
	free(data_section);
}

void etap3(Elf32_Ehdr* ehdr, FILE * fp, char *str, int flag){
	int numero = -1, count = 0; 
	
  	ehdr = read_header_ELF(fp, flag);
    
    	count = ehdr->e_shnum;    

    	Elf32_Shdr *shdr = read_Section_header(fp, ehdr, flag);
    
    	char *shstrtab = read_sh_str_tab(fp, shdr, ehdr->e_shstrndx);

	char *temp = shstrtab;
	
	numero = atoi(str);
	if (str[0] != 0 && numero == 0)
		numero = -1;
	
	if (numero > -1) 
		temp += shdr[numero].sh_name;
	else
		numero = chercher_index_de_section(shdr, shstrtab, count, str);
	
	affichage_contenu_section(numero, temp, shdr, fp);
  	printf("\n");
  	free(shdr);
}
