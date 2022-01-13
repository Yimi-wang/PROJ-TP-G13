#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>
#include <getopt.h>
#include "Etap6.h"
#include "Etap7.h"
#include "Etap8_9.h"
#include "Etap11.h"

#define MAXSIZE 2

void usage(char *name) {
	fprintf(stderr, "Usage:\n"
	"%s [--section-start section_name=value(héxa)] [-o file_product] file_original\n" 
		, name);
}

char *get_nom_rel_corre(char *name_source) {
	char *new_name = (char *)malloc(sizeof(name_source)+sizeof(char)*5);
	strcpy(new_name, ".rel");
	strcat(new_name, name_source);
	return new_name;
}

FILE *init_new_file(FILE *fp_original, char *new_filename) {
	FILE *fp_out;
	unsigned int a = 0;
	
	fp_out = fopen(new_filename, "w+"); 

	uint8_t *bintemp = (uint8_t *)malloc(sizeof(uint8_t)*1024);

	/* Copier les données de la source à la sortie */
	fseek(fp_original, 0, SEEK_SET);
	while(!feof(fp_original)) {
		a = fread(bintemp, 1, 1024, fp_original);
		assert(a != 0);
		if (a != 1024) 
			a = fwrite(bintemp, a, 1, fp_out);
		else
			a = fwrite(bintemp, 1024, 1, fp_out);
		assert(a >= 1);
	}

	free(bintemp);

	return fp_out;
}

int main(int argc, char *argv[]) {
	int opt = 0, index = 0, flag = 0;
	uint32_t adresse_donnee[MAXSIZE];
	FILE *fp, *fp_sortie = NULL;
	char str[20];
	char **str_section = NULL;
	char *str_valeur = NULL, *nom_rel_corre = NULL, *nom_file_exe = NULL;
	char *delim = "=";
	
	str_section = (char **)malloc(sizeof(char *)*MAXSIZE);
	
	printf("file: %s\n", argv[argc - 1]);
	if (argc < 3) {
		usage(argv[0]);
		exit(1);
	}
	
	fp = fopen(argv[argc - 1], "rb");
	if(fp == NULL) {
		printf("Erreur d'ouverture du fichier: %s\n", argv[argc - 1]);
		exit(1);
	}
	
	struct option longopts[] = {
		{ "section-start", required_argument, NULL, 's' },
		{ NULL, 0, NULL, 0 }
	};
	
	while ((opt = getopt_long(argc, argv, "s:o:", longopts, NULL)) != -1) {
		switch (opt) {
			case 's':
				str_section[index] = strtok(optarg, delim);
				printf("Section à charger: %s, ", str_section[index]);
				str_valeur = strtok(NULL, delim);
				if (str_valeur[0] != '0' || str_valeur[1] != 'x') {
					printf("Wrong format of value: %s\n", str_valeur);
					usage(argv[0]);
					break;
				}
				adresse_donnee[index] = strtol(str_valeur, NULL, 16);
				printf("avec une valeur (adresse): 0x%x\n",adresse_donnee[index]);
				index++;
				break;
			
			case 'o':
				printf("Réimplantations dans le fichier: %s\n", optarg);
				nom_file_exe = strdup(optarg);
				fp_sortie = init_new_file(fp, "temp");
				break;
			
			default :
				fprintf(stderr, "Unrecognized option %c\n", optopt);
				usage(argv[0]);
				exit(1);
		}
	}

	fseek(fp, 0, SEEK_SET);
	fread(str, 1, 6, fp);
	if(str[0] != 0x7f || str[1] != 'E' || str[2] != 'L' || str[3] != 'F') {
		printf("%s n'est pas un fichier ELF.\n", argv[argc - 1]);
		exit(1);
	}  
	
	if(str[5] == 2) {
		printf("%s est compilé en boutisme big endian.\n", argv[argc - 1]);
		printf("Une inversion du lecture des donnée est nécessaire.\n");
		flag = 1;
	}
	
	for (int i = 0; i < MAXSIZE; i++) 
		addr_charge(fp_sortie, str_section[i], adresse_donnee[i], flag);
		
	proc_reimplants(fp_sortie, flag);

	for (int j = 0; j < MAXSIZE; j++) {
		nom_rel_corre = get_nom_rel_corre(str_section[j]);
		printf("--Supprimer la section: %s\n", nom_rel_corre);
		int x = supprimer_une_section(fp_sortie, nom_rel_corre, flag);
		if (x >= 0)
			corrige_ndx(fp_sortie, x, flag);
		else
			printf("---Il n'y a pas de section %s.\n", nom_rel_corre);	
	}
	
	printf("Réimplantations est bien finies.\n");
	
	printf("Produire d’un fichier exécutable.\n");
	produ_executable(fp_sortie, flag, nom_file_exe);
	printf("finish.\n");
	
	free(nom_rel_corre);
	free(str_section);
	fclose(fp);
	fclose(fp_sortie);
	return 0;
}
