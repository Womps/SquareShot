/** Projet 'Jeu de tir vintage en C avec SDL'
*
* #squareshot.c
*
* EPIARD Romain
* FAIVRE Ophélie
* PELTIER Thomas
* CARRO Maxime
* GAUTIER Dylan
*
* #Compilation: gcc -O3 projet.c -o projet `sdl-config --cflags --libs` -lSDL_ttf
* #Compilation(sous windows) : gcc -O3 squareshot.c -o squareshot.exe -lmingw32 -lSDLmain -lSDL -lSDL_ttf -mwindows
*
* #Equivalent options:
  --cflags=" -D_GNU_SOURCE=1 -Dmain=SDL_main"
  --libs=" -lmingw32 -lSDLmain -lSDL  -mwindows"
  --staticlibs=" -lmingw32 -lSDLmain -lSDL  -mwindows  -lm -luser32 -lgdi32 -lwinmm -ldxguid"
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "SDL/SDL.h"
#include <math.h>
#include <time.h>
#include "images.c"
#include "files.c"
#include "themes.c"
#include "colors.c"
#include "display.c"
#include "target.c"
#include "game.c"
#include "cursor.c"
#include "functions.c"
#include "play.c"
#include "mainMenu.c"
#include "finalMenu.c"
#include "confirmationMenu.c"

#define DEBUG 0
#if __WIN32__
#define OUT_CONSOLE "CON"
#else
#define OUT_CONSOLE "/dev/tty"
#endif
#define SEP "/"
#define LEVELS_PATH "levels"
#define LEVELS_NB_FILEPATH "levels/n.ini"
#define DEFAULT_ICON "images/squareshot.ico"
#define DEFAULT_FONT "fonts/PressStart2P.ttf"


//***************************************************************************
//                            	MAIN
//***************************************************************************
int main(int argc, char *argv[]) {

#if !DEBUG &&  __WIN32__ // si on est dans la version "release" sous windows, on enlève les flux d'entrée et de sortie (évite la création de fichiers comme stdout.txt, etc)
	reopen(" ",true);
	reopen(" ",false);
#endif

	//--------------------------------------------------------------------------
	//                        	Déclarations
	//--------------------------------------------------------------------------
	DisplayParameters display;
	GameParameters game;
	Cursor cursor;
	Screen screen;
	Target target;
	Theme thm;

	GameExitCode exitCode;
	GenericMenuCode genRetCode;
	SettingsMenuCode settingsRetCode;
	HighScoresMenuCode highScoresRetCode;//création d'une variable highScoresRetCode de type HighScoresMenuCode, pour le code de retour du menu HighScores
	ConfirmationMenuCode confirmationMenuRetCode;
	FinalMenuCode finalMenuRetCode;

	int level;
	int nblevel;
	bool shwMainMenu;
	bool continueGame;
	char *mainMenuMsg=NULL;
	char levelName[strlen("Niveau 9999 / 9999")+ 1];
	char levelPath[strlen(LEVELS_PATH) + strlen(SEP) + strlen("level9999") + strlen(SEP) + 1 ];



	//--------------------------------------------------------------------------
	//                 Traitement des paramètres de commande
	//--------------------------------------------------------------------------
	int i=0;
	bool listModes=false;
	unsigned int fullScreen=0;
	int width=DISPLAY_DEFAULT_WIDTH;
	int height=DISPLAY_DEFAULT_HEIGHT;
	int framePerSec=DISPLAY_DEFAULT_FPS;
	int colorDepth=DISPLAY_DEFAULT_COLORDEPTH;
	for(i=1; i<argc; i+=2){
		if(strcmp(argv[i], "-con")==0){reopen(OUT_CONSOLE, true); reopen(OUT_CONSOLE, false);}
		else if(strcmp(argv[i], "-in")==0) reopen(argv[i+1], false);
		else if(strcmp(argv[i], "-out")==0) reopen(argv[i+1], true);
		else if(strcmp(argv[i], "-w")==0) width=atoi(argv[i+1]);
		else if(strcmp(argv[i], "-h")==0) height=atoi(argv[i+1]);
		else if(strcmp(argv[i], "-c")==0) colorDepth=atoi(argv[i+1]);
		else if(strcmp(argv[i], "-fps")==0) framePerSec=atoi(argv[i+1]);
		else if(strcmp(argv[i], "-fullscreen")==0) {fullScreen=SDL_FULLSCREEN; i--;}
		else if(strcmp(argv[i], "-list")==0) {listModes=true; i--;}
		else printf("'%s' n'est pas un paramètre valide:\n[-w WIDTH] [-h HEIGHT] [-c COLORDEPTH] [-fps FPS] [-fullscreen]\n[-list] [-out STREAM_NAME] [-in STREAM_NAME] [-con]\n", argv[i]);
	}
	if(listModes){
		SDL_Init(SDL_INIT_VIDEO);
		printModes();
		SDL_Quit();
		return 0;
	}


	//--------------------------------------------------------------------------
	//                        	Initialisations
	//--------------------------------------------------------------------------
	srand(time(0));;

	if(SDL_Init(SDL_INIT_VIDEO)==-1){//Initialisation de SDL
		fprintf(stderr, "Erreur : Initialisation de SDL\n");
		exit(EXIT_FAILURE);
	}
	SDL_WM_SetCaption("Squareshot", "Squareshot");	// titre fenêtre
	SDL_WM_SetIcon(SDL_LoadBMP(DEFAULT_ICON), NULL); // icone de la fenetre

	if(TTF_Init()==-1){
		fprintf(stderr, "Erreur :Initialisation de SDL_ttf\n");
		exit(EXIT_FAILURE);
	}

	initScreen(&screen);
	initDisplayParameters(&display, width, height, colorDepth, framePerSec, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_HWACCEL | fullScreen);
	if(!setDisplayParameters(&display, &screen)){
	    	printf("Erreur : Impossible de creer la surface 'ecran'");
	    	SDL_Quit();
	    	return 1;
	}

	initTarget(&target, &display, TARGET_DEFAULT_SIZE);
	initTheme(&thm, makeColor(16, 4, 16), makeColor(255, 255, 255), makeColor(200, 200, 200), DEFAULT_FONT, false, NULL, 0);
	setTheme(&thm, screen.background, target.surface);

	initCursor(&cursor, makeColor(0, 0, 255), CURSOR_DEFAULT_FILENAME);
	if(setCursor(&cursor, &display)){
		SDL_ShowCursor(0);
	}else{
		printf("Erreur : Impossible de charger le curseur principal");
		cursor.dec->x=0;
		cursor.dec->y=0;
	}

	FILE* nbLevelFile=fopen(LEVELS_NB_FILEPATH, "r");
	if (nbLevelFile==NULL){
        printf("Erreur : Impossible de lire le fichier des niveaux");
	    SDL_Quit();
	    return 1;
    }
	fscanf(nbLevelFile, "nbLevels=%d\n", &nblevel);//Obtention du nombre de niveaux :
	fclose(nbLevelFile);

	initGamePlayer2Parameters(&game, false, 500, 1000);

    game.score=0; //initialisation du score à 0
	game.player2Score=0; //initialisation du score J2 à 0
    game.difficulty=2; //initialisation de la difficulté à 2 (Moyenne)

	FILE *fichier = NULL; //on initialise le fichier des scores s'il n'est pas présent ou ouvrable (ce qui inclue sa création)
	fichier = fopen(fichier_score,"r");
	if (fichier == NULL) {init_score();}
	else {fclose(fichier);}
	//fin modif
	//--------------------------------------------------------------------------
	//                        		Jeu
	//--------------------------------------------------------------------------
	do{
		shwMainMenu=true;
		continueGame=true;
		exitCode=GAME_EXIT_NONE;
		genRetCode=GENERIC_MENU_NONE;
		settingsRetCode=SETTINGS_MENU_NONE;
		highScoresRetCode=HIGH_SCORES_MENU_NONE;
		finalMenuRetCode=FINAL_MENU_NONE;
		switch(showMainMenu(&game/* modif */, &screen, &display, &cursor, &thm, mainMenuMsg)){
			case MAIN_MENU_EXIT:
				continueGame=false;
				exitCode=GAME_EXIT_QUIT;
				break;
			case MAIN_MENU_1_PLAYER:
				game.twoPlayers=false;
				break;
			case MAIN_MENU_2_PLAYERS:
				game.twoPlayers=true;
				break;
			case MAIN_MENU_SETTINGS://initialisation du score à 0
				do{
					settingsRetCode=showSettingsMenu(&screen, &display, &cursor, NULL, &target, &thm, &game);
				}while(settingsRetCode==SETTINGS_MENU_RELOAD);
				if(settingsRetCode==SETTINGS_MENU_QUIT){
					exitCode=GAME_EXIT_QUIT;
				}else{
					exitCode=GAME_EXIT_MENU;
				}
				continueGame=false;
				break;
            case MAIN_MENU_HIGH_SCORES:
                do{
                    highScoresRetCode=showHighScoresMenu(&screen, &display, &cursor, &thm, &game);//affichage du menu HighScores
                    }while(highScoresRetCode==HIGH_SCORES_MENU_RELOAD);
                    if(highScoresRetCode==HIGH_SCORES_MENU_EXIT)
                    {
                        exitCode=GAME_EXIT_MENU;
                    }else if(highScoresRetCode==HIGH_SCORES_MENU_CONFIRMATION){
				    confirmationMenuRetCode=CONFIRMATION_MENU_NONE;
				    do{
				    confirmationMenuRetCode=showConfirmationMenu(&screen,&display,&cursor,&thm,&game);
				}while(confirmationMenuRetCode==CONFIRMATION_MENU_NONE);
				if(confirmationMenuRetCode==CONFIRMATION_MENU_EXIT)
                {
                    exitCode=GAME_EXIT_MENU;
                }
                else{
                    exitCode=GAME_EXIT_QUIT;
                }
                    }
                    else
                    {
                        exitCode=GAME_EXIT_QUIT;
                    }
                continueGame=false;
                break;
			default:
				continueGame=false;
				break;
		}
		mainMenuMsg=NULL;

		//chargement successif des niveaux
		//------------------------------------------------------------------------------------------------------------
		for(level=1; (level<=nblevel) && continueGame; level++){
			sprintf(levelName,"Niveau %d / %d",level, nblevel);
			sprintf(levelPath, "%s%slevel%d%s", LEVELS_PATH, SEP, level, SEP);
			initGameParameters(&game, (double)level, 20.0*(1.1-(double)(level/nblevel)), 10.0 + (double)level, level, levelPath);
			exitCode=play(&game, &display, &screen, &cursor, &target, &thm, levelName);

			if(((exitCode & GAME_EXIT_CODE_MASK) | GAME_EXIT_WARNING) != GAME_EXIT_STANDARD_WARNING){
				continueGame=false;
			}
			if(exitCode & GAME_EXIT_WARNING){			// s'il y a un avertissement
				if((exitCode & GAME_EXIT_CODE_MASK)==GAME_EXIT_ERROR) printf("Erreur : ");
				else if((exitCode & GAME_EXIT_CODE_MASK)==GAME_EXIT_EXCEPTION) printf("Exception : ");
				else printf("Avertissement : ");
				printf("Niveau %d : Code de retour : %d, Message : %d\n", level, exitCode, exitCode & GAME_EXIT_INFO_MASK);
			}else if(exitCode & GAME_EXIT_INFO_MASK){	// sinon s'il y a un message
				printf("Niveau %d , Message : %d\n", level, exitCode & GAME_EXIT_INFO_MASK);
			}

		}// fin boucle niveaux
		//------------------------------------------------------------------------------------------------------------
		switch((exitCode & GAME_EXIT_CODE_MASK) | GAME_EXIT_WARNING){
			case GAME_EXIT_STANDARD_WARNING: //Réussite
                if (game.twoPlayers==false)
                {
                    char pseudo[16];
                    finalMenuRetCode=showFinalMenu(&game, &screen, &display, &cursor, &thm, pseudo, 15);//affichage du menu final
                    if (finalMenuRetCode==FINAL_MENU_CONTINUE) //on demande son pseudo au joueur et on quitte le jeu au besoin
                    {
                        ecriture_score(game.score,pseudo);
                        highScoresRetCode=showHighScoresMenu(&screen, &display, &cursor, &thm, &game);//affichage du menu HighScores
                        if(highScoresRetCode==HIGH_SCORES_MENU_EXIT)
                        {
                        exitCode=GAME_EXIT_MENU;
                        }else if(highScoresRetCode==HIGH_SCORES_MENU_CONFIRMATION){
				    confirmationMenuRetCode=CONFIRMATION_MENU_NONE;
				    do{
				    confirmationMenuRetCode=showConfirmationMenu(&screen,&display,&cursor,&thm,&game);
				}while(confirmationMenuRetCode==CONFIRMATION_MENU_NONE);
				if(confirmationMenuRetCode==CONFIRMATION_MENU_EXIT)
                {
                    exitCode=GAME_EXIT_MENU;
                }
                else{
                    exitCode=GAME_EXIT_QUIT;
                }
                    }
                        else
                        {
                        exitCode=GAME_EXIT_QUIT;
                        }
                    }
                    else{
                        exitCode=GAME_EXIT_QUIT;
                    }
                }
                else
                {
                    genRetCode=showGenericMenu(&game, &screen, &display, &cursor, &thm, "FIN", "Felicitations !", NULL);
                }
                game.score=0;
				game.player2Score=0;
				break;
			case GAME_EXIT_ESCAPE_WARNING: //Abandon : afficher message puis quitter
				genRetCode=showGenericMenu(&game, &screen, &display, &cursor, &thm, "ABANDON", "Demain est un autre jour !", NULL);
                game.score=0;
				game.player2Score=0;
				break;
			case GAME_EXIT_MENU_WARNING: // Retour au menu principal
				break;
			case GAME_EXIT_EXCEPTION: // Erreur empechant chargement d'une partie (fichier manquant,...)
				mainMenuMsg="(Attention, présence d'une erreur, vérifiez vos fichiers !)";
				break;
			case GAME_EXIT_ERROR:
				SDL_Quit();
				return 1;
				break;
			default: /*GAME_EXIT_NONE, GAME_EXIT_QUIT, (GAME_EXIT_ESCAPE | GAME_EXIT_WARNING), GAME_EXIT_WARNING*/
				shwMainMenu=false;
				break;
		}
		if(genRetCode==GENERIC_MENU_QUIT) shwMainMenu=false;
	}while(shwMainMenu);



	//--------------------------------------------------------------------------
	//                            	FIN
	//--------------------------------------------------------------------------
	SDL_FreeSurface(screen.window);
	SDL_FreeSurface(screen.background);
	SDL_FreeSurface(target.surface);
	TTF_Quit();
	SDL_Quit();
	return EXIT_SUCCESS;
}

