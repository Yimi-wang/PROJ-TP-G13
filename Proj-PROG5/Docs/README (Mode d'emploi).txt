Mode d'emploi
	Si votre machine contient des outils automake/autoconf, vous pouvez vous installez dans le dossier elf_linker-1.0 pour compiler notre programme par les commandes:
		./configure 
		make

	Sinon, pour compiler les programmes pour phase 1, il faut aller dans le dossier Proj-PROG5 et compiler par:
		cd Prog-PROG5
		cd Phase_1
		make
	Pour compiler les programmes pour phase 2, il faut aller dans le dossier Proj-PROG5 et compiler par:
		cd Prog-PROG5
		cd Phase_2
		make

	Pour lancer les programmes pour phase 1, il doit compiler et utiliser  la command :
		./Pharse1 [-option / –numéro_étape / –index(d’étape)] nom_de_fichier
	Ici c’est la list de option:
		-h / –etape1 / –1 : Affichage de l'en-tête (header ELF) 
		-S / –etape2 / –2 : Affichage du contenu de la table de sections     							-x / –etape3 / –3 [numéro]: Affichage du contenu d'une section
	pour affichage d’une section, il faut donner un index ou un nom donc il doit utiliser la commande suivante:
		./Pharse3 -x index_ou_nom_de_section nom_de_fichier
		-s / –etape4 / –4 : Affichage de la table des symboles (etap 4)
		-r / –etape5 / –5 : Affichage des tables de réimplantation (etap 5)
		-H : Affichage la list des aides

Pour lancer les programmes pour phase 2, il doit compiler et utiliser  la command :
		./Pharse2 [-option] nom_de_fichier
	Ici c’est la list de option:
		-s / --section-start [nom_section]=[valeur / adresse (hexa)]:
 			 Charger les adresse données à chaque section correspondante.
		-o nom_fichier :
			 Produire un fichier ELF exécutable par le nom donnée: 

Scripts de test automatiques :
	- autop1.sh pour la Phase 1:
	on exécute avec le nom du fichier à tester (sans .o) et autant de numéros d’étapes que l’on souhaite.

	exemple: ./autop1 example1 1 2 5

	- autop2.sh pour la Phase 2:
	on exécute avec le nom du fichier à tester (sans .o), le fichier .o correspondant sera compilé. Le contenu des sections .text et .data seront affichés grâce à objdump

	exemple: ./autop2 example1
