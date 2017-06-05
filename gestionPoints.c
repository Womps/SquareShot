/*
     Partie de programme : Syst�me de comptage de points
     Fonctions : Calcul des points / Sauvegarde de points dans un fichier / lecture points / Cryptage fichier / D�cryptage fichier / R�initialisation des points / saisie du pseudo du joueur
//////////////////////////////////////////////////
Ces programmes servent � simuler l'ajout, la lecture, etc... des valeur des scores dans un fichier pr�vu � cet effet
Ce fichier devra se nomer "score.txt"
le fichier sera initialis� avec 9 lignes (car 9 scores de sauvegard� maximum), la syntaxe sera la suivante :
0 void (par d�faut, "0" est le score et "void" est le pseudo)
Ils seront peu � peu remplac� par les scores des joueurs au fur et � mesure du jeu.
//////////////////////////////////////////////////
Il y a aussi en deuxi�me partie les fonctions propre au cryptage/d�cryptage du fichier voulu, nous avons choisi de
crypter ce fichier de facon � ce que des utilisateurs malins ne puissent pas essayer de modifier les scores � la
main en modifiant directement le fichier avec un �diteur de texte
Nous avons donc r�alis� un algorithme de cryptage par mot cl�, c'est � dire que chaque caract�re
du fichier (seulement les caract�res ASCII inscriptibles) est d�cal� d'un certain nombre de fois, selon le mot cl� entr�,
pour le premier caract�re on tien compte du premier caract�re du mot cl�, pour le deuxi�me ... , et une fois qu'on est �
la fin du mot cl� on revien au d�but de celui-ci et on recommence de la m�me facon jusqu'a la fin du fichier.
///////////////////////////////////////////////////
Il y a aussi une fonction permetant de r�cup�rer le pseudo du joueur une foie qu'il a fini sa partie.
On regarde donc un par un chaques caract�res qui sont entr� sur le clavier, on les sauvegades dans une chaine de caract�re
qu'on affiche en temps r�el dans la fen�tre du jeu.
///////////////////////////////////////////////////
*/
#ifndef GESTIONPOINTS
#define GESTIONPOINTS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>    // ne surtout pas oublier d'inclure cette biblioth�que
#include <SDL/SDL.h>

typedef struct string_char    //construction de ce type pour pouvoir utiliser des tableaux de chaine de caract�re
{
    char chaine[16];
} string_char;

//Variables globales du fichier :
char motcle[] = "maxime";   //mot cl� pour le cryptage et d�cryptage par mot cl�
char fichier_score[] = "score.txt";  //fichier utilis� pour sauvegarder les scores

//prototypes des fonctions : (regroup�s par th�mes)
int calcul_points (int niveau, int temps, int nb_clics);

void init_score ();
void ecriture_score (int score, char pseudo[]);
void lire_score (int ligne, int *points, int pseudo[]);

void crypter (char motclee[]);
void decrypter (char motclee[]);
int decalage(int nb,int i);





/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
//Fonction qui calcul le nombre de points r�alis� � chaque tir r�ussit :
//Les param�tres sont le niveau auquel le joueur est (1,2 ou 3) le temps est le temps que le joueur � mis
//pour toucher la cible et nb_clics est le nombre de clics que le joueur � r�alis� avant de toucher la cible
//La fonction retourne le nombre de points que le joueur aura r�alis�
int calcul_points (int niveau, int temps, int nb_clics)
{
    int points=0;
    int coef_niveau=500000;
    points = ((niveau*coef_niveau)/(temps)) - ((nb_clics/1000000));
    return points;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// Fonctions pour la gestion des scores /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
//fonction pour �crire un score dans le fichier, il suffit de donner le nombre de points r�alis�s et la chaine
//de caract�res contenant le pseudo du joueur et la fonction fait toute seule le calcul de la place o� l'attribuer
void ecriture_score (int score, char pseudo[])
{
    decrypter(motcle);
    FILE *fichier = NULL;
    fichier = fopen(fichier_score, "r+");
    int tab_score[11];
    string_char tab_pseudo[11];
    int i = 1;

    if (fichier != NULL)
    {
        while ((fscanf(fichier, "%d %s", &tab_score[i], tab_pseudo[i].chaine) != 0) && (i<=10))
        {
            i++;
        }
        rewind(fichier);
        i = 1;
        int bool_test = 0;
        while (i<10)
        {
            if ((score>=tab_score[i]) && (bool_test==0))
            {
                fprintf(fichier,"%d %s\n", score, pseudo);
                bool_test = 1;
            }
            else
            {
                fprintf(fichier,"%d %s\n", tab_score[i], tab_pseudo[i].chaine);
                i++;
            }
        }

        fclose(fichier);
    }
    crypter(motcle);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
void init_score ()                // fonction pour r�initialiser les points (efface tout et initialise � "0 void")
{
    FILE *fichier = NULL;
    fichier = fopen(fichier_score, "w+");
    int i=1;
    if (fichier != NULL)
    {
        while (i<=10)
        {
            fprintf(fichier,"0 void\n");
            i++;
        }

        fclose(fichier);
    }
    crypter(motcle);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
void lire_score (int ligne, int *points, int pseudo[])// fonction qui sert � lire les points contenus dans le fichier des scores
{                                                    //elle prend en param�tres le num�ro de ligne que l'on souhaite lire du fichier
    decrypter(motcle);                              //mais aussi les variable dans lesquelles on enregistrera le r�sultat (donc le pseudo voulu et les points associ�s)
    FILE *fichier = NULL;
    fichier = fopen(fichier_score, "r");
    if (fichier != NULL)
    {
        while (ligne>0)
        {
            fscanf(fichier,"%d %s", points, pseudo);
            ligne--;
        }
        fclose(fichier);

    }
    crypter(motcle);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
///////////// Fonctions propre au cryptage et d�cryptage du fichier de scores ////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
void crypter (char motclee[])
{
    int j=0;
    FILE *fichier = NULL;
    fichier = fopen(fichier_score,"r+");
    if (fichier != NULL)
    {
        int car;
        car = fgetc(fichier);
        int i = 0;

        int tab_car[350];
        while (i<=349)
        {
            tab_car[i]=0;
            i++;
        }

        i=0;
        while (car != EOF )      //la boucle sert � lire chaque caract�res, ensuite on les crypte un par un et on les mets temporairement dans un tableau
        {
            if (car>32  && car<127)
            {
                tab_car[i]=decalage(car,motclee[j]-32);
                j++;
                if (j>=strlen(motclee))      {j=0;}
            }
            else
            {
                tab_car[i]=car;
            }
            i++;
            fseek(fichier, 0, SEEK_CUR);
            car = fgetc(fichier);
        }


        i=0;
        rewind(fichier);        // on retourne au d�but du fichier afin de r�crire tout ce qui se trouve dans le tableau de caract�res
        while (tab_car[i] != 0 )
        {
            fputc(tab_car[i],fichier);
            i++;
        }
    }
    fclose(fichier);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
void decrypter (char motclee[])
{
    int j=0;
    FILE *fichier = NULL;
    fichier = fopen(fichier_score,"r+");
    if (fichier != NULL)
    {
        int car;
        car = fgetc(fichier);
        int i = 0;

        int tab_car[350];
        while (i<=349)
        {
            tab_car[i]=0;
            i++;
        }

        i=0;
        while (car != EOF )      //la boucle sert � lire chaque caract�res, ensuite on les d�crypte un par un et on les mets temporairement dans un tableau
        {
            if (car>32  && car<127)
            {
                tab_car[i]=decalage(car,-1*(motclee[j]-32));
                j++;
                if (j>=strlen(motclee))      {j=0;}
            }
            else
            {
                tab_car[i]=car;
            }
            i++;
            fseek(fichier, 0, SEEK_CUR);
            car = fgetc(fichier);
        }


        i=0;
        rewind(fichier);       // on retourne au d�but du fichier afin de r�crire tout ce qui se trouve dans le tableau de caract�res
        while (tab_car[i] != 0 )
        {
            fputc(tab_car[i],fichier);
            i++;
        }
    }
    fclose(fichier);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
int decalage(int nb,int i)      // fonction que sert � d�caler les caract�res ASCII lors du cryptage
{
    if (nb>32 && nb<127) //on ne le d�cale que s'il fait partie de la plage des caract�res ASCII inscriptibles (non �tendue)
    {
        nb+=i;
        if (nb>126)
        {
            nb-=94;
        }
        else if (nb<33)
        {
            nb+=94;
        }
    }
    return nb;
}
#endif
