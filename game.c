/**
* # game.c
*/

#ifndef GAME
#define GAME
#include <stdbool.h>

typedef struct GameParameters{
	double speed;    	// Coefficient multiplicateur pour la vitesse appliqué lors du calcul de la position (ex: x = x + (cos(theta) * speed))
	double coeff;    	// Coefficient pour l'augmentation de la vitesse en fonction du nombre de points (speed=nbPoints/coeff)
	int pointsNumber;   // Nombre total de points du niveau actuel
	int pointsLimit;	// Nombre de points maximal dans le niveau actuel
	int level;    		// Niveau actuel
	char* levelPath;	// Chemin d'acces au dossier du niveau (afin de charger les sons, images, paramètres,...)
	bool twoPlayers;
	int player2PointsNumber;
	int player2KBSensitivity;
	int player2KBMaxSensitivty;  // Nb max "d'évennements"/"de tours" pour un déplacement du curseur
	int score;// Score du joueur 1
	int player2Score;// Score du joueur 2
    int commandes;// Pour gérer le type de commandes sélectionnée
    int difficulty;// Pour gérer le niveau de difficulté
    double coeff_diff;// Pour gérer comment la vitesse varie selon le niveau (moyen = difficile)
}GameParameters;


typedef enum{
	// CODE:
	GAME_EXIT_CODE_MASK=0x0F,	//1111
								//+------- Erreur
								//|+------ Menu
								//||+----- Fin du jeu
								//|||+---- Fin de partie
	GAME_EXIT_NONE=0x00,		//0000  : Jeu en cours, pas de sortie
	GAME_EXIT_STANDARD=0x01,	//0001  : Fin de partie, niveau terminé
	GAME_EXIT_ESCAPE=0x02,		//0010  : Quitter après appui sur ECHAP = Abandon
	GAME_EXIT_QUIT=0x03,		//0011  : Fermeture par Alt+F4, croix de la fenêtre, etc, sortie immédiate
	GAME_EXIT_MENU=0x04,		//0100	: Retour menu principale
	GAME_EXIT_WARNING=0x08,		//1000	: Avertissement, "Erreur légère" prise en charge, pas de sortie
	GAME_EXIT_EXCEPTION=0x0D,	//1101  : Exception, fin de partie -> retour menu
	GAME_EXIT_ERROR=0x0E,		//1110  : Erreur grave, sortie du programme
	GAME_EXIT_STANDARD_WARNING=0x09,	//1001
	GAME_EXIT_ESCAPE_WARNING=0x0A,		//1010
	GAME_EXIT_QUIT_WARNING=0x0B,		//1011
	GAME_EXIT_MENU_WARNING=0x0C,		//1100
	// INFO:
	GAME_EXIT_INFO_MASK=0xF0,		//11110000
									//+------- Fichier non présent
									//|+------ Problème son FMOD
									//||+----- Info1
									//|||+---- Info2
	GAME_EXIT_INFO_GAMEFILE=0x90,	//10010000 (144) : Fichier de paramètre du jeu non accessible
	GAME_EXIT_INFO_BACKGROUND=0xA0  //10100000 (160): Impossible de creer/dessiner le fond (probablement image arrière plan inaccessible)

} GameExitCode;


//***************************************************************************
//                    	initGameParameters
// Procédure permettant d'initialiser les valeur d'une structure de type
//                    	'GameParameters'
//***************************************************************************
void initGameParameters(GameParameters* params, double speed, double coeff, int pointsLimit, int level, char* levelPath){
	params->speed=speed;
	params->coeff=coeff;
	params->pointsNumber=0;
	params->pointsLimit=pointsLimit;
	params->level=level;
	params->levelPath=levelPath;
	params->player2PointsNumber=0;
}

void initGamePlayer2Parameters(GameParameters* params, bool twoPlayers, int kBSensitivity, int kBMaxSensitivty){
	if(kBSensitivity > kBMaxSensitivty) kBSensitivity = kBMaxSensitivty;
	params->twoPlayers=twoPlayers;
	params->player2KBSensitivity=kBSensitivity;
	params->player2KBMaxSensitivty=kBMaxSensitivty;
	params->commandes=1;//initialisation de la commande à 1 (Souris/Clavier)
	params->difficulty=2;//initialisation de la difficulté à 2 (Moyenne)
	params->coeff_diff=2;//initialisation du coefficient ajouté à la vitesse à 10 (Moyenne)
	params->score=0;//initialisation du score à 0
	params->player2Score=0;//initialisation du score du joueur 2 à 0
}

//*********************************************************************************
//                            	incPoints
//	* Incrémente le total de points dans une structure 'GameParameters'
//	* Augmente la vitesse en fonction de ce nombre de points
//	* Indique si le total des points est égale à la limite du niveau
//*********************************************************************************
bool incPoints(GameParameters* game, bool player2){
	if(player2){
		game->speed += (double)(++(game->player2PointsNumber))/(game->coeff);
		if((game->player2PointsNumber)==(game->pointsLimit)) return true;
		else return false;
	}else{
		game->speed += (double)(++(game->pointsNumber))/(game->coeff);
		if((game->pointsNumber)==(game->pointsLimit)) return true;
		else return false;
	}
}
#endif
