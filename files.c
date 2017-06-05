/**
* # files.c
*/

#include <stdio.h>

#ifndef FILES
#define FILES
#include <stdlib.h>
#include <stdbool.h>


//*********************************************************************************
//                            	fileExists
//			Fonction permettant de savoir si un fichier existe
//*********************************************************************************
bool fileExists(char* path){
	bool exists;
	FILE* file=fopen(path,"r");
	if(file!=NULL) exists=true;
	else exists=false;
	fclose(file);
	return exists;
}


#endif


	/*	FILE* fopen(const char* nomDuFichier, const char* modeOuverture);
			"r" : lecture seule.
			"w" : �criture seule.
			"a" : mode d'ajout.
			"r+" : lecture et �criture, le fichier doit avoir �t� cr�� au pr�alable.
			"w+" : lecture et �criture, avec suppression du contenu au pr�alable
			"a+" : ajout en lecture / �criture � la fin.

		int fclose(FILE* pointeurSurFichier);
		int fputc(int caractere, FILE* pointeurSurFichier);
		int fputs(const char* chaine, FILE* pointeurSurFichier);
		fprintf(FILE* pointeurSurFichier, "chaine format�e %d ", parametre)

		int fgetc(FILE* pointeurDeFichier);
		char* fgets(char* chaine, int nombreDeCaracteresALire, FILE* pointeurSurFichier); // s'arrete au premier \n : lecture ligne par ligne
		//-> while (fgets(chaine, TAILLE_MAX, fichier) != NULL)
		fscanf(fichier, "%d %d %d ...", pointeur1, pointeur2, ...);

		long ftell(FILE* pointeurSurFichier); // position dans le fichier
		int fseek(FILE* pointeurSurFichier, long deplacement, int origine);
			SEEK_SET : indique le d�but du fichier.
			SEEK_CUR : indique la position actuelle du curseur.
			SEEK_END : indique la fin du fichier.
		void rewind(FILE* pointeurSurFichier); // revenir au d�but
		int rename(const char* ancienNom, const char* nouveauNom);
		int remove(const char* fichierASupprimer);

		fclose(fichier);

		bzero(buffer, sizeof(buffer));
		strcat(buffer,"chaine");
		sprintf(buffer, "%format1 %format2 ...", donn�es1, donn�es2, ...);

	*/
