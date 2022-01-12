#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>
#include <getopt.h>
#include "Etap1.h"
#include "Etap2.h"
#include "Etap3.h"
#include "Etap4.h"
#include "Etap5.h"

void usage(char *name) {
	fprintf(stderr, "Usage:\n"
	"%s [options / --numéro_étape] fichier\n" 
	"Options:\n"
	"\t-h: Affichage de l’en-tête (header ELF).\n"
	"\t-S: Affichage de la table des sections (Section header table).\n"
	"\t-x nom/index: Affichage du contenu d'une section correspondante.\n"
	"\t-s: Affichage de la table des symboles.\n"
	"\t-r: Affichage des tables de réimplantation du fichier.\n"
		, name);
}

int main(int argc, char *argv[]) {
	int opt = 0, flag = 0;
	FILE *fp;
	char str[20];
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)malloc(sizeof(Elf32_Ehdr));

	fp = fopen(argv[argc - 1], "rb");
	if(fp == NULL) {
		printf("Erreur d'ouverture du fichier: %s\n", argv[argc - 1]);
		exit(1);
	}

	fread(str, 1, 6, fp);
	if(str[0] != 0x7f || str[1] != 'E' || str[2] != 'L' || str[3] != 'F') {
		printf("%s n'est pas un fichier ELF.\n", argv[argc - 1]);
		exit(1);
	}  
	
	if(str[5] == 2) {
		printf("%s est compilé en boutisme big endian. ", argv[argc - 1]);
		printf("Une inversion du lecture des donnée est nécessaire.\n");
		flag = 1;
	}
	
	struct option longopts[] = {
		{ "etape1", no_argument, NULL, 'h' },
		{ "1", no_argument, NULL, 'h' },
		{ "etape2", no_argument, NULL, 'S' },
		{ "2", no_argument, NULL, 'S' },
		{ "etape3", required_argument, NULL, 'x' },
		{ "3", required_argument, NULL, 'x' },
		{ "etape4", no_argument, NULL, 's' },
		{ "4", no_argument, NULL, 's' },
		{ "etape5", no_argument, NULL, 'r' },
		{ "5", no_argument, NULL, 'r' },
		{ NULL, 0, NULL, 0 }
	};
	
	while ((opt = getopt_long(argc, argv, "hSx:sr", longopts, NULL)) != -1) {
		switch (opt){
			case 'h':
				printf("Affichage de l'en-tête (header ELF) de '%s':\n", argv[argc - 1]);
				etap1(ehdr,fp,flag);
				break;

			case 'S':
				printf("Affichage de la table des sections (Section header table) de '%s':\n", argv[argc - 1]);
				etap2(ehdr,fp,flag);
				break;

			case 'x':
				if (argc > 3) {
					printf("Affichage du contenu d'une section du fichier de %s: \n", argv[argc - 1]);
					etap3(ehdr,fp,optarg,flag);
				}	
				else
					usage(argv[0]);
				break;

			case 's':
				printf("Affichage de la table des symboles du fichier %s: \n", argv[argc - 1]);
				etap4(ehdr,fp,flag);
				break;

			case 'r':
				printf("Affichage des tables de réimplantation du fichier %s: \n", argv[argc - 1]);
				etap5(ehdr,fp,flag);
				break;

			default:
				fprintf(stderr, "Unrecognized option %c\n", optopt);
				usage(argv[0]);
				fclose(fp);
				exit(1);
		}
	}
	fclose(fp);
	return 0;
}
