#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>
#include <getopt.h>
#include "Etap6.h"
#include "Etap7.h"
#include "Etap8_9.h"

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

void generer_new_file(FILE *fp_sortie, uint32_t *adresse_donnee, char **str_section) {
	int *index_rels = NULL;
	char *nom_rel_corre = NULL;	

	for (int i = 0; i < MAXSIZE; i++) 
		addr_charge(fp_sortie, str_section[i], adresse_donnee[i]);

	proc_reimplants(fp_sortie);

	for (int j = 0; j < MAXSIZE; j++) {
		nom_rel_corre = get_nom_rel_corre(str_section[j]);
		index_rels[j] = supprimer_une_section(fp_sortie, nom_rel_corre);
		corrige_ndx(fp_sortie, index_rels[j]);
	}
		
	free(nom_rel_corre);
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
	int opt = 0, index = 0;
	uint32_t adresse_donnee[MAXSIZE];
	FILE *fp, *fp_sortie;
	char str[20];
	char **str_section = NULL;
	char *str_valeur = NULL, *nom_rel_corre = NULL;
	char *delim = "=";
	
	printf("file: %s\n", argv[argc - 1]);
	struct option longopts[] = {
		{ "section-start", required_argument, NULL, 's' },
		{ NULL, 0, NULL, 0 }
	};
	
	str_section = (char **)malloc(sizeof(char *)*MAXSIZE);
	
	fp = fopen(argv[argc - 1], "rb");
	if(fp == NULL) {
		printf("Erreur d'ouverture du fichier: %s\n", argv[argc - 1]);
		exit(1);
	}

	fread(str, 1, 5, fp);
	if(str[0] != 0x7f || str[1] != 'E' || str[2] != 'L' || str[3] != 'F') {
		printf("%s n'est pas un fichier ELF.\n", argv[argc - 1]);
		exit(1);
	}  
	
	while ((opt = getopt_long(argc, argv, "s:o:", longopts, NULL)) != -1) {
		switch (opt) {
			case 's':
				printf("--section-start: %s\n", optarg);
				str_section[index] = strtok(optarg, delim);
				str_valeur = strtok(NULL, delim);
				if (str_valeur[0] != '0' || str_valeur[1] != 'x') {
					printf("Wrong format of value: %s\n", str_valeur);
					usage(argv[0]);
					break;
				}
				adresse_donnee[index] = strtol(str_valeur, NULL, 16);
				index++;
				break;
			
			case 'o':
				printf("-o: %s\n", optarg);
				fp_sortie = init_new_file(fp, optarg);
				break;
			
			default :
				fprintf(stderr, "Unrecognized option %c\n", optopt);
				usage(argv[0]);
				fclose(fp);
				fclose(fp_sortie);
				exit(1);
		}
	}
	
	if (fp_sortie != NULL) {
		for (int i = 0; i < MAXSIZE; i++) 
			addr_charge(fp_sortie, str_section[i], adresse_donnee[i]);

		proc_reimplants(fp_sortie);

		for (int j = 0; j < MAXSIZE; j++) {
			nom_rel_corre = get_nom_rel_corre(str_section[j]);
			int x = supprimer_une_section(fp_sortie, nom_rel_corre);
			corrige_ndx(fp_sortie, x);
		}
		
		free(nom_rel_corre);
		printf("Réimplantations est fait dans le fichier:%s\n", optarg);
	}
	
	free(str_section);
	fclose(fp);
	fclose(fp_sortie);
	return 0;
}
