/**
* # play.c
*/

#ifndef PLAY
#define PLAY

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <SDL/SDL.h>
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
#include "genericMenu.c"
#include "pauseMenu.c"
#include "settingsMenu.c"
#include "gestionPoints.c"


//***************************************************************************
//                            	play1
//    	Lance une nouvelle partie selon les paramètres de 'game'
//	sur la "fenetre" 'screen' avec les paramètres d'affichage 'display'
//***************************************************************************
GameExitCode play1(GameParameters* game, DisplayParameters* display, Screen* screen, Cursor* cursor, Target* target, Theme* thm){


	//---------------------------------------------------------------------------
	//                        	Déclarations
	//---------------------------------------------------------------------------
	double theta, nextx, nexty, x, y;
	SettingsMenuCode settingsRetCode;
	PauseMenuCode pauseCode;
	GameExitCode gameState;
	unsigned int nextTime;
	bool waitSyncTime;
	SDL_Event event;
	Theme menuThm;
	bool canCount;
	bool pause;

	int pathLen = strlen(game->levelPath);
	const char gameFileName[]="game.ini";
	char gameSettingsPath[pathLen + sizeof(gameFileName)];
	FILE* gameSettingsFile=NULL;
	char imagePath[pathLen + 31];
	char musicPath[pathLen + 31];
	char useMusic;

	// Déclarations pour le joystick
    if ( SDL_Init( SDL_INIT_JOYSTICK ) < 0 )
        return EXIT_FAILURE;
    SDL_Joystick *j1;
    j1 = SDL_JoystickOpen(0);
    SDL_JoystickEventState(SDL_ENABLE);

	//---------------------------------------------------------------------------
	//                        	Initialisations/Calculs initiaux
	//---------------------------------------------------------------------------
	gameState = GAME_EXIT_NONE;
	canCount = true;
	pause = false;

	theta = ((double)(rand())/RAND_MAX)*(2 * M_PI) - M_PI;
	x = rand()%(screen->window->w - target->surface->w);
	y = rand()%(screen->window->h - target->surface->h);
	target->location.x = (int)x;
	target->location.y = (int)y;

	initTheme(&menuThm, thm->backColor, thm->targetColor, thm->foreColor, thm->fontPath, false, NULL,0);

	sprintf(gameSettingsPath, "%s%s", game->levelPath, gameFileName);
	sprintf(imagePath, "%s", game->levelPath);
	sprintf(musicPath, "%s", game->levelPath);

	gameSettingsFile = fopen(gameSettingsPath, "r");
	if(gameSettingsFile==NULL) return (GAME_EXIT_EXCEPTION | GAME_EXIT_INFO_GAMEFILE);
	fscanf(gameSettingsFile, "speed=%lf\ncoeff=%lf\npointsLimit=%d\n", &(game->speed), &(game->coeff), &(game->pointsLimit));
	fscanf(gameSettingsFile, "useBackgroundImage=%c\nimageLayout=%d\nimageName=%s\n", &(thm->useImage),&(thm->imageLayout), &imagePath[pathLen]);
	fscanf(gameSettingsFile, "useMusic=%c\nmusicName=%s\n", &useMusic, &musicPath[pathLen]);
	fclose(gameSettingsFile);

	thm->imagePath=imagePath;
	if(!setThemeBackground(thm, screen->background)) gameState |= (GAME_EXIT_WARNING | GAME_EXIT_INFO_BACKGROUND);


	//---------------------------------------------------------------------------
	//                     Texte : Déclarations et initialisations
	//---------------------------------------------------------------------------
	char level_Str[15];
	char score_Str[30];
	char points_Str[30];

	SDL_Color color={thm->foreColor->R, thm->foreColor->G, thm->foreColor->B};
	TTF_Font* mainFont = TTF_OpenFont(thm->fontPath, 20);

	sprintf(level_Str, "Niveau : %d", game->level);
	sprintf(score_Str, "Nombre de carres : %d / %d", game->pointsNumber,game->pointsLimit);
	sprintf(points_Str, "Score : %d", game->score);

	SDL_Surface* level_Surface=TTF_RenderText_Solid(mainFont, level_Str, color);
	SDL_Surface* score_Surface=TTF_RenderText_Solid(mainFont, score_Str, color);
	SDL_Surface* points_Surface=TTF_RenderText_Solid(mainFont, points_Str, color);

	SDL_Rect level_Rect={10, 10 ,0 , 0};
	SDL_Rect score_Rect={10, level_Rect.y + level_Surface->h + 10, 0, 0};
	SDL_Rect points_Rect={10, score_Rect.y + score_Surface->h + 10, 0, 0};

    int tempsPrecedent = 0;//Pour calculer le temps
    int tempsActuel = 0;//Pour calculer le temps
    int tempsToucherCible = 0;//Pour calculer le temps
    int nbClic = 0; //Pour calculer le nombre de clics


	//---------------------------------------------------------------------------
	//                         Affichage Initial
	//---------------------------------------------------------------------------
	printf("----- Niveau %d -----\n", game->level);
	SDL_BlitSurface(screen->background, NULL, screen->window, NULL);
	SDL_BlitSurface(level_Surface, NULL, screen->window, &level_Rect);
	SDL_BlitSurface(score_Surface, NULL, screen->window, &score_Rect);
	SDL_BlitSurface(points_Surface, NULL, screen->window, &points_Rect);
	SDL_BlitSurface(target->surface, NULL, screen->window, &target->location);
	//Pour l'utilisation de la souris
	if(game->commandes == 1) {
        SDL_GetMouseState(&(cursor->location->x), &(cursor->location->y));
	}
	//ou du joystick
	else {
        (cursor->location->x) = 320;
        (cursor->location->y) = 240;
	}
	drawCursor(cursor, screen->window);
	SDL_Flip(screen->window);


	//---------------------------------------------------------------------------
	//                        	boucle principale
	//---------------------------------------------------------------------------
	while(!(gameState & (~GAME_EXIT_WARNING & GAME_EXIT_CODE_MASK))){
	    	//--------------------------------------------------------------------------
	    	//                    	Déplacement de la cible
	    	//--------------------------------------------------------------------------
	    	// on calcule la nouvelle position
	    	nextx = x + (cos(theta) * (game->speed + game->coeff_diff));
	    	nexty = y + (sin(theta) * (game->speed + game->coeff_diff));
	    	if ((nextx < 0) || (nextx > (screen->window->w - target->surface->w))) {
			theta = M_PI - theta;
			nextx = x + (cos(theta) * (game->speed + game->coeff_diff));
	    	}
	    	if ((nexty < 0) || (nexty > (screen->window->h - target->surface->h))) {
			theta = - theta;
			nexty = y + (sin(theta) * (game->speed + game->coeff_diff));
	    	}
	    	x=nextx;
	    	y=nexty;
	    	target->location.x = (int)(x) ;
	    	target->location.y = (int)(y) ;
	    	// on redessine l'arrière plan (on efface la cible)
	    	SDL_BlitSurface(screen->background, NULL, screen->window, NULL);  //carré changement demain rappelle timer
			// On affiche le texte
			SDL_BlitSurface(level_Surface, NULL, screen->window, &level_Rect);
			SDL_BlitSurface(score_Surface, NULL, screen->window, &score_Rect);
			SDL_BlitSurface(points_Surface, NULL, screen->window, &points_Rect);
	    	// on place la surface blanche à la nouvelle position (on dessine dans le buffer)
	    	SDL_BlitSurface(target->surface, NULL, screen->window, &target->location);

			drawCursor(cursor, screen->window);

	    	//--------------------------------------------------------------------------
	    	//                    	Affichage/Synchronisation
	    	//--------------------------------------------------------------------------
	    	// On place le contenu du buffer dans la mémoire vidéo (on affiche le contenu du buffer)
	    	SDL_Flip(screen->window);
	    	// On récupère ensuite "l'heure" actuelle à laquelle on rajoute un temps d'attente
	    	nextTime = SDL_GetTicks() + display->syncTime;
			// On indique que l'on doit attendre un certain temps avant de refaire l'affichage
	    	waitSyncTime=true;
	    	// puis on boucle jusqu'à ce ce temps soit écoulé, et on gère les évennements en même temps :
			// si un évennement tel qu'un clique sur la cible ou l'appui sur echap se produit,
			// on traite directement l'évennement avec les actions qui y sont liées, et waitSyncTime
			// prend la valeur false ce qui a pour effet de quitter directement la boucle d'attente
			// ===> réactivité maximale !


	    	//--------------------------------------------------------------------------
	    	//                        	Gestion des évènements
	    	//--------------------------------------------------------------------------
	    	do{
	    		SDL_PollEvent(&event);
				if(event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_ESCAPE){
					if(!pause){
						pause=true;
						do{
							pauseCode=showPauseMenu(screen, display, cursor, &menuThm, game->level,game);
							if(pauseCode==PAUSE_MENU_SETTINGS){
								do{
									settingsRetCode=showSettingsMenu(screen, display, cursor, NULL, target, &menuThm, game);
									if(target->location.x != (int)x){	// s'il y a eu un changement de résolution
	    								x = (double)target->location.x ; // on "repositionne" la cible
	    								y = (double)target->location.y ;
									}
								}while(settingsRetCode==SETTINGS_MENU_RELOAD);
								if(settingsRetCode==SETTINGS_MENU_QUIT){
									pauseCode=PAUSE_MENU_QUIT;
									gameState|=GAME_EXIT_QUIT;
								}
							}
							else if(pauseCode==PAUSE_MENU_EXIT){
								gameState|=GAME_EXIT_ESCAPE;
							}
							else if(pauseCode==PAUSE_MENU_QUIT){
								gameState|=GAME_EXIT_QUIT;
							}
						}while(pauseCode==PAUSE_MENU_SETTINGS);
						setThemeBackground(thm, screen->background);
						waitSyncTime=false;
					}
				}else if(pause) pause=false;
                tempsActuel = SDL_GetTicks(); //prise du temps
	    		if(game->commandes == 1) {
                    switch(event.type){
				case SDL_MOUSEMOTION:
					SDL_GetMouseState(&(cursor->location->x), &(cursor->location->y));
					break;
	    		case SDL_QUIT:
	    			gameState|=GAME_EXIT_QUIT;
					waitSyncTime=false;
			    	break;
				case SDL_MOUSEBUTTONDOWN:
					SDL_GetMouseState(&(cursor->location->x), &(cursor->location->y));
					if(contains(&target->location, cursor->location->x, cursor->location->y) && canCount){
                            //On compare la position du rectangle à celle de notre curseur, et si la valeur renvoyée est true, alors la conditoon est remplie.

                        tempsToucherCible = tempsActuel - tempsPrecedent;//Calcul du temps que l'on a mis à toucher la cible
                        tempsPrecedent = tempsActuel; // Le temps "actuel" devient le temps "precedent" pour nos futurs calculs de temps

						game->score = game->score + calcul_points (game->level,tempsToucherCible, nbClic);//On incrémente le nombre de point du nombre de points calculé par la fonction

						if(incPoints(game,false)){
							printf("%d Points : Niveau terminé !\n", game->pointsNumber);
							gameState|=GAME_EXIT_STANDARD;
							waitSyncTime=false;
						}else{
							printf("Un point de plus! :%d\nNouvelle vitesse : %f\n", game->pointsNumber, game->speed);
							theta = ((double)(rand())/RAND_MAX)*(2 * M_PI) - M_PI;
							x = rand()%(screen->window->w - target->surface->w);
							y = rand()%(screen->window->h - target->surface->h);
							waitSyncTime=false;
						}
						explode(target, screen, game, cursor, NULL, display, thm, 4, 5);
						sprintf(score_Str, "Nombre de carres : %d / %d", game->pointsNumber ,game->pointsLimit);
						sprintf(points_Str, "Score : %d ", game->score);
						SDL_FreeSurface(score_Surface);
						SDL_FreeSurface(points_Surface);
						score_Surface=TTF_RenderText_Solid(mainFont, score_Str, color);
						points_Surface=TTF_RenderText_Solid(mainFont,points_Str,color);
			    		nbClic=0; //Reinitialise le nombre de clics a chaque fois que le carré explose
			    	}
			    	else
			    	{
			    	    nbClic++; //Compteur du nombre de clics
			    	}

					canCount=false;
			    	break ;
				case SDL_MOUSEBUTTONUP:
					canCount=true;
					break;
				default : break;
		    	}
	    		}
	    		else {
                    switch(event.type){
                    case SDL_JOYAXISMOTION:
                        if((SDL_JoystickGetAxis(j1,0) / 100 + 320) < 0) (cursor->location->x) = 0;
                        else (cursor->location->x) = SDL_JoystickGetAxis(j1,0) / 100 + 320;
                        if((SDL_JoystickGetAxis(j1,1) / 100 + 240) < 0) (cursor->location->y) = 0;
                        else (cursor->location->y) = SDL_JoystickGetAxis(j1,1) / 100 + 240;
                        if((SDL_JoystickGetAxis(j1,0) / 100 + 320) > 640) (cursor->location->x) = 640;
                        if((SDL_JoystickGetAxis(j1,1) / 100 + 240) > 480) (cursor->location->y) = 480;
                        break;
                    case SDL_QUIT:
                        gameState|=GAME_EXIT_QUIT;
                        waitSyncTime=false;
                        break;
                    case SDL_JOYBUTTONDOWN:
                        (cursor->location->x) = SDL_JoystickGetAxis(j1, 0) / 100 + 320;
                        (cursor->location->y) = SDL_JoystickGetAxis(j1, 1) / 100 + 240;
                        if(contains(&target->location, cursor->location->x, cursor->location->y) && canCount){
                        //On compare la position du rectangle à celle de notre curseur, et si la valeur renvoyée est true, alors la conditoon est remplie.

                        tempsToucherCible = tempsActuel - tempsPrecedent;//Pour calculer le temps
                        tempsPrecedent = tempsActuel; // Le temps "actuel" devient le temps "precedent" pour nos futurs calculs de temps

						game->score = game->score + calcul_points (game->level,tempsToucherCible, nbClic);//On incrémente le nombre de point du nombre de points calculé par la fonction
                            if(incPoints(game,false)){
                                printf("%d Points : Niveau terminé !\n", game->pointsNumber);
                                gameState|=GAME_EXIT_STANDARD;
                                waitSyncTime=false;
                            }else{
                                printf("Un point de plus! :%d\nNouvelle vitesse : %f\n", game->pointsNumber, game->speed);
                                theta = ((double)(rand())/RAND_MAX)*(2 * M_PI) - M_PI;
                                x = rand()%(screen->window->w - target->surface->w);
                                y = rand()%(screen->window->h - target->surface->h);
                                waitSyncTime=false;
                            }
                            explode(target, screen, game, cursor, NULL, display, thm, 4, 5);
                            sprintf(score_Str, "Nombre de carres : %d / %d", game->pointsNumber ,game->pointsLimit);
                            sprintf(points_Str, "Score : %d", game->score);
                            SDL_FreeSurface(score_Surface);
                            SDL_FreeSurface(points_Surface);
                            score_Surface=TTF_RenderText_Solid(mainFont, score_Str, color);
                            points_Surface=TTF_RenderText_Solid(mainFont, points_Str, color);
                            nbClic=0; //Reinitialise le nombre de clics a chaque fois que le carré explose
			    	}
			    	else
			    	{
			    	    nbClic++; //Compteur du nombre de clics
			    	}
                        canCount=false;
                        break ;
                    case SDL_JOYBUTTONUP:
                        canCount=true;
                        break;
                    default : break;
                    }
	    		}

		}while(waitSyncTime && (SDL_GetTicks() < nextTime));
	} // Fin boucle principale

	SDL_FreeSurface(level_Surface);
	SDL_FreeSurface(score_Surface);
	SDL_FreeSurface(points_Surface);
	TTF_CloseFont(mainFont);
	if(game->commandes == 2) SDL_JoystickClose(j1);
	return gameState; // On retourne le code de sortie
}




//***************************************************************************
//                            	play2
//  Lance une nouvelle partie 2 joueurs selon les paramètres de 'game'
//	sur la "fenetre" 'screen' avec les paramètres d'affichage 'display'
//***************************************************************************
GameExitCode play2(GameParameters* game, DisplayParameters* display, Screen* screen, Cursor* cursor, Target* target, Theme* thm){

	//---------------------------------------------------------------------------
	//                        	Déclarations
	//---------------------------------------------------------------------------
	double theta, nextx, nexty, x, y;
	SettingsMenuCode settingsRetCode;
	GenericMenuCode genRetCode;
	PauseMenuCode pauseCode;
	GameExitCode gameState;
	unsigned int nextTime;
	bool waitSyncTime;
	SDL_Event event;
	Theme menuThm;
	bool canCount1;
	bool canCount2;
	bool pause;

	Cursor* cursor1;
	Cursor* cursor2;
	int cur2Direction = 0x00;
	int cur2CountX = 0;
	int cur2CountY = 0;


	int pathLen = strlen(game->levelPath);
	const char gameFileName[]="game.ini";
	char gameSettingsPath[pathLen + sizeof(gameFileName)];
	FILE* gameSettingsFile=NULL;
	char imagePath[pathLen + 31];
	char musicPath[pathLen + 31];
	char useMusic;

	int tempsPrecedent = 0;//Pour calculer le temps
    int tempsActuel = 0;//Pour calculer le temps
    int tempsToucherCible = 0;//Pour calculer le temps
    int nbClic = 0; //Pour calculer le nombre de clics


    if ( SDL_Init( SDL_INIT_JOYSTICK ) < 0 )
        return EXIT_FAILURE;
    SDL_Joystick *j1;
    SDL_Joystick *j2;
    j1 = SDL_JoystickOpen(0);
    j2 = SDL_JoystickOpen(1);
    SDL_JoystickEventState(SDL_ENABLE);


	//---------------------------------------------------------------------------
	//                        	Initialisations/Calculs initiaux
	//---------------------------------------------------------------------------
	gameState = GAME_EXIT_NONE;
	canCount1=true;
	canCount2=true;
	pause = false;

	theta = ((double)(rand())/RAND_MAX)*(2 * M_PI) - M_PI;
	x = rand()%(screen->window->w - target->surface->w);
	y = rand()%(screen->window->h - target->surface->h);
	target->location.x = (int)x;
	target->location.y = (int)y;

	initTheme(&menuThm, thm->backColor, thm->targetColor, thm->foreColor, thm->fontPath, false, NULL,0);

	cursor1=makeCursor(CURSOR_PLAYER1_FILENAME, makeColor(0, 0, 255), display);
	cursor2=makeCursor(CURSOR_PLAYER2_FILENAME, makeColor(0, 0, 255), display);

	sprintf(gameSettingsPath, "%s%s", game->levelPath, gameFileName);
	sprintf(imagePath, "%s", game->levelPath);
	sprintf(musicPath, "%s", game->levelPath);

	gameSettingsFile = fopen(gameSettingsPath, "r");
	if(gameSettingsFile==NULL) return (GAME_EXIT_EXCEPTION | GAME_EXIT_INFO_GAMEFILE);
	fscanf(gameSettingsFile, "speed=%lf\ncoeff=%lf\npointsLimit=%d\n", &(game->speed), &(game->coeff), &(game->pointsLimit));
	fscanf(gameSettingsFile, "useBackgroundImage=%c\nimageLayout=%d\nimageName=%s\n", &(thm->useImage),&(thm->imageLayout), &imagePath[pathLen]);
	fscanf(gameSettingsFile, "useMusic=%c\nmusicName=%s\n", &useMusic, &musicPath[pathLen]);
	fclose(gameSettingsFile);

	thm->imagePath=imagePath;
	if(!setThemeBackground(thm, screen->background)) gameState |= (GAME_EXIT_WARNING | GAME_EXIT_INFO_BACKGROUND);



	//---------------------------------------------------------------------------
	//                     Texte : Déclarations et initialisations
	//---------------------------------------------------------------------------
	char level_Str[15];
	char score1_Str[20];
	char score2_Str[20];
	char point1_Str[20];
	char point2_Str[20];
	SDL_Color color={thm->foreColor->R, thm->foreColor->G, thm->foreColor->B};
	TTF_Font* mainFont = TTF_OpenFont(thm->fontPath, 20);
	sprintf(level_Str, "Niveau : %d", game->level);
	sprintf(score1_Str, "Joueur 1 : %d / %d", game->pointsNumber ,game->pointsLimit);
	sprintf(score2_Str, "Joueur 2 : %d / %d", game->player2PointsNumber ,game->pointsLimit);
	sprintf(point1_Str, "Score J1 : %d", game->score);
	sprintf(point2_Str, "Score J2 : %d", game->player2Score);

	SDL_Surface* level_Surface=TTF_RenderText_Solid(mainFont, level_Str, color);
	SDL_Surface* score1_Surface=TTF_RenderText_Solid(mainFont, score1_Str, color);
	SDL_Surface* score2_Surface=TTF_RenderText_Solid(mainFont, score2_Str, color);
	SDL_Surface* point1_Surface=TTF_RenderText_Solid(mainFont, point1_Str, color);
	SDL_Surface* point2_Surface=TTF_RenderText_Solid(mainFont, point2_Str, color);

	SDL_Rect level_Rect={10, 10 ,0 , 0};
	SDL_Rect score1_Rect={10, level_Rect.y + level_Surface->h + 10, 0, 0};
	SDL_Rect point1_Rect={10, score1_Rect.y + score1_Surface->h + 10, 0, 0};
	SDL_Rect score2_Rect={10, point1_Rect.y + point1_Surface->h + 10, 0, 0};
	SDL_Rect point2_Rect={10, score2_Rect.y + score2_Surface->h + 10, 0, 0};

	//---------------------------------------------------------------------------
	//                         Affichage Initial
	//---------------------------------------------------------------------------
	printf("----- Niveau %d -----\n", game->level);
	SDL_BlitSurface(screen->background, NULL, screen->window, NULL);
	SDL_BlitSurface(level_Surface, NULL, screen->window, &level_Rect);
    SDL_BlitSurface(score1_Surface, NULL, screen->window, &score1_Rect);
	SDL_BlitSurface(score2_Surface, NULL, screen->window, &score2_Rect);
	SDL_BlitSurface(point1_Surface, NULL, screen->window, &point1_Rect);
	SDL_BlitSurface(point2_Surface, NULL, screen->window, &point2_Rect);

	SDL_BlitSurface(target->surface, NULL, screen->window, &target->location);

	if(game->commandes == 1) {
        SDL_GetMouseState(&(cursor1->location->x), &(cursor1->location->y));
	}
	else {
        (cursor1->location->x) = 300;
        (cursor1->location->y) = 240;
	}

	drawCursor(cursor1, screen->window);

	if(game->commandes == 1) {
        setCursorLoc(cursor2, rand()%(screen->window->h - cursor2->surface->w), rand()%(screen->window->h - cursor2->surface->h));
    }
    else {
        (cursor2->location->x) = 340;
        (cursor2->location->y) = 240;
    }

	drawCursor(cursor2, screen->window);
	SDL_Flip(screen->window);


	//---------------------------------------------------------------------------
	//                        	boucle principale
	//---------------------------------------------------------------------------
	while(!(gameState & (~GAME_EXIT_WARNING & GAME_EXIT_CODE_MASK))){

			//--------------------------------------------------------------------------
	    	//                    	Déplacement de la cible
	    	//--------------------------------------------------------------------------
	    	nextx = x + (cos(theta) * (game->speed + game->coeff_diff));
	    	nexty = y + (sin(theta) * (game->speed + game->coeff_diff));
	    	if ((nextx < 0) || (nextx > (screen->window->w - target->surface->w))) {
			theta = M_PI - theta;
			nextx = x + (cos(theta) * (game->speed + game->coeff_diff));
	    	}
	    	if ((nexty < 0) || (nexty > (screen->window->h - target->surface->h))) {
			theta = - theta;
			nexty = y + (sin(theta) * (game->speed + game->coeff_diff));
	    	}
	    	x=nextx;
	    	y=nexty;
	    	target->location.x = (int)(x) ;
	    	target->location.y = (int)(y) ;

	    	SDL_BlitSurface(screen->background, NULL, screen->window, NULL);
			SDL_BlitSurface(level_Surface, NULL, screen->window, &level_Rect);
			SDL_BlitSurface(score1_Surface, NULL, screen->window, &score1_Rect);
			SDL_BlitSurface(score2_Surface, NULL, screen->window, &score2_Rect);
			SDL_BlitSurface(point1_Surface, NULL, screen->window, &point1_Rect);
			SDL_BlitSurface(point2_Surface, NULL, screen->window, &point2_Rect);
	    	SDL_BlitSurface(target->surface, NULL, screen->window, &target->location);
			drawCursor(cursor1, screen->window);
			drawCursor(cursor2, screen->window);

	    	//--------------------------------------------------------------------------
	    	//                    	Affichage/Synchronisation
	    	//--------------------------------------------------------------------------
	    	SDL_Flip(screen->window);
	    	nextTime = SDL_GetTicks() + display->syncTime;
			waitSyncTime=true;

	    	//--------------------------------------------------------------------------
	    	//                        	Gestion des évènements
	    	//--------------------------------------------------------------------------
	    	do{
	    		SDL_PollEvent(&event);
				if(event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_ESCAPE){
					if(!pause){
						pause=true;
						do{
							pauseCode=showPauseMenu(screen, display, cursor, &menuThm, game->level,game);
							if(pauseCode==PAUSE_MENU_SETTINGS){
								do{
									settingsRetCode=showSettingsMenu(screen, display, cursor, cursor2, target, &menuThm, game);
									if(target->location.x != (int)x){
	    								x = (double)target->location.x ;
	    								y = (double)target->location.y ;
									}
								}while(settingsRetCode==SETTINGS_MENU_RELOAD);
								if(settingsRetCode==SETTINGS_MENU_QUIT){
									pauseCode=PAUSE_MENU_QUIT;
									gameState|=GAME_EXIT_QUIT;
								}
							}
							else if(pauseCode==PAUSE_MENU_EXIT){
								gameState|=GAME_EXIT_ESCAPE;
							}
							else if(pauseCode==PAUSE_MENU_QUIT){
								gameState|=GAME_EXIT_QUIT;
							}
						}while(pauseCode==PAUSE_MENU_SETTINGS);
						setThemeBackground(thm, screen->background);
						waitSyncTime=false;
					}
				}else pause=false;
                tempsActuel = SDL_GetTicks(); //prise du temps
               //passage au mode souris
	    		if(game->commandes == 1) {
	    		    switch(event.type){
				case SDL_MOUSEMOTION:
					SDL_GetMouseState(&(cursor1->location->x), &(cursor1->location->y));
					break;
	    		case SDL_QUIT:
	    			gameState|=GAME_EXIT_QUIT;
					waitSyncTime=false;
			    	break;
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym)
					{
					case SDLK_SPACE:
						if(contains(&target->location, cursor2->location->x, cursor2->location->y) && canCount2){
                        tempsToucherCible = tempsActuel - tempsPrecedent;//Pour calculer le temps
                        tempsPrecedent = tempsActuel; // Le temps "actuel" devient le temps "precedent" pour nos futurs calculs de temps

						game->player2Score = game->player2Score + calcul_points (game->level,tempsToucherCible, nbClic);

							if(incPoints(game,true)){
								explode(target, screen, game, cursor1, cursor2, display, thm, 4, 5); // doit etre appelée ici avant les menus
								char winStr[50];
								sprintf(winStr,"( avec %d points contre %d points )", game->player2PointsNumber, game->pointsNumber);
							    genRetCode=showGenericMenu(game, screen, display, cursor, &menuThm, "Niveau terminé !"," Gagnant : joueur 2 ", winStr);
								printf("Niveau terminé ! Gagnant: joueur 2 avec %d points contre %d pour le joueur 1\n", game->player2PointsNumber, game->pointsNumber);
								if(genRetCode==GENERIC_MENU_QUIT) gameState|=GAME_EXIT_QUIT;
								else gameState|=GAME_EXIT_STANDARD;
								waitSyncTime=false;
							}else{
								explode(target, screen, game, cursor1, cursor2, display, thm, 4, 5);
								printf("Joueur 2 marque un point :%d\nNouvelle vitesse : %f\n", game->player2PointsNumber, game->speed);
								theta = ((double)(rand())/RAND_MAX)*(2 * M_PI) - M_PI;
								x = rand()%(screen->window->w - target->surface->w);
								y = rand()%(screen->window->h - target->surface->h);
								waitSyncTime=false;
							}
							sprintf(score2_Str, "Joueur 2 : %d / %d", game->player2PointsNumber ,game->pointsLimit);
							sprintf(point2_Str, "Score J2 : %d" , game->player2Score);
							SDL_FreeSurface(score2_Surface);
							SDL_FreeSurface(point2_Surface);
							score2_Surface=TTF_RenderText_Solid(mainFont, score2_Str, color);
							point2_Surface=TTF_RenderText_Solid(mainFont, point2_Str, color);

	    					nbClic=0; //Reinitialise le nombre de clics a chaque fois que le carré explose
	    				}
                        else
                        {
								nbClic++; //Compteur du nombre de clics
                        }
						canCount2=false;
						break;
					case SDLK_UP:	 cur2Direction = (cur2Direction & ~0x2) | 0x1; break;
					case SDLK_DOWN:  cur2Direction = (cur2Direction & ~0x1) | 0x2; break;
					case SDLK_RIGHT: cur2Direction = (cur2Direction & ~0x8) | 0x4; break;
					case SDLK_LEFT:	 cur2Direction = (cur2Direction & ~0x4) | 0x8; break;
					default : break;
					}
					break ;
				case SDL_KEYUP:
					switch(event.key.keysym.sym){
					case SDLK_SPACE: canCount2 = true; break;		// réactive le clique pour le curseur 2
					case SDLK_UP:	 cur2Direction &= ~0x1; break;	// désactive le déplacement vers le haut du curseur 2
					case SDLK_DOWN:	 cur2Direction &= ~0x2; break;	// désactive le déplacement vers le bas du curseur 2
					case SDLK_RIGHT: cur2Direction &= ~0x4; break;	// désactive le déplacement vers la droite du curseur 2
					case SDLK_LEFT:  cur2Direction &= ~0x8; break;	// désactive le déplacement vers la gauche du curseur 2
					default : break;
					}
					break ;
				case SDL_MOUSEBUTTONDOWN:
					SDL_GetMouseState(&(cursor1->location->x), &(cursor1->location->y));
					if(contains(&target->location, cursor1->location->x, cursor1->location->y) && canCount1){
						tempsToucherCible = tempsActuel - tempsPrecedent;//Pour calculer le temps
                        tempsPrecedent = tempsActuel; // Le temps "actuel" devient le temps "precedent" pour nos futurs calculs de temps
						game->score = game->score + calcul_points (game->level,tempsToucherCible, nbClic);//calcul des points à ajouter

						if(incPoints(game,false)){
							explode(target, screen, game, cursor1, cursor2, display, thm, 4, 5);
							char winStr[50];
							sprintf(winStr,"( avec %d points contre %d points )", game->pointsNumber, game->player2PointsNumber);
							genRetCode=showGenericMenu(game, screen, display, cursor, &menuThm, "Niveau terminé !"," Gagnant : joueur 1", winStr);
							printf("Niveau terminé ! Gagnant: joueur 1 avec %d points contre %d pour le joueur 2\n", game->pointsNumber, game->player2PointsNumber);
							if(genRetCode==GENERIC_MENU_QUIT) gameState|=GAME_EXIT_QUIT;
							else gameState|=GAME_EXIT_STANDARD;
							waitSyncTime=false;
						}else{
							explode(target, screen, game, cursor1, cursor2, display, thm, 4, 5);
							printf("Joueur 1 marque un point :%d\nNouvelle vitesse : %f\n", game->pointsNumber, game->speed);
							theta = ((double)(rand())/RAND_MAX)*(2 * M_PI) - M_PI;
							x = rand()%(screen->window->w - target->surface->w);
							y = rand()%(screen->window->h - target->surface->h);
							waitSyncTime=false;
						}
						sprintf(score1_Str, "Joueur 1 : %d / %d", game->pointsNumber ,game->pointsLimit);
						sprintf(point1_Str, "Score J1 : %d ", game->score);

						SDL_FreeSurface(score1_Surface);
						SDL_FreeSurface(point1_Surface);

						score1_Surface=TTF_RenderText_Solid(mainFont, score1_Str, color);
						point1_Surface=TTF_RenderText_Solid(mainFont, point1_Str, color);



			    		nbClic=0; //Reinitialise le nombre de clics a chaque fois que le carré explose
			    	}
						else
						{
							nbClic++; //Compteur du nombre de clics
						}
					canCount1=false;
				   	break ;
				case SDL_MOUSEBUTTONUP:
					canCount1=true;
					break;
				default :
					break;
		    	}
	    		}

	    		else {
                    switch(event.type){
                    case SDL_JOYAXISMOTION:
                    if((SDL_JoystickGetAxis(j1,0) / 100 + 300) < 0) (cursor1->location->x) = 0;
                    else (cursor1->location->x) = SDL_JoystickGetAxis(j1,0) / 100 + 300;
                    if((SDL_JoystickGetAxis(j1,1) / 100 + 240) < 0) (cursor1->location->y) = 0;
                    else (cursor1->location->y) = SDL_JoystickGetAxis(j1,1) / 100 + 240;
                    if((SDL_JoystickGetAxis(j1,0) / 100 + 300) > 640) (cursor1->location->x) = 640;
                    if((SDL_JoystickGetAxis(j1,1) / 100 + 240) > 480) (cursor1->location->y) = 480;

                    if((SDL_JoystickGetAxis(j2,0) / 100 + 340) < 0) (cursor2->location->x) = 0;
                    else (cursor2->location->x) = SDL_JoystickGetAxis(j2,0) / 100 + 340;
                    if((SDL_JoystickGetAxis(j2,1) / 100 + 240) < 0) (cursor2->location->y) = 0;
                    else (cursor2->location->y) = SDL_JoystickGetAxis(j2,1) / 100 + 240;
                    if((SDL_JoystickGetAxis(j2,0) / 100 + 340) > 640) (cursor2->location->x) = 640;
                    if((SDL_JoystickGetAxis(j2,1) / 100 + 240) > 480) (cursor2->location->y) = 480;
					break;
	    		case SDL_QUIT:
	    			gameState|=GAME_EXIT_QUIT;
					waitSyncTime=false;
			    	break;
				case SDL_JOYBUTTONDOWN:
					if(SDL_JoystickGetButton(j2,2) == 1) {
						if((SDL_JoystickGetAxis(j2,0) / 100 + 340) < 0) (cursor2->location->x) = 0;
                        else (cursor2->location->x) = SDL_JoystickGetAxis(j2,0) / 100 + 340;
                        if((SDL_JoystickGetAxis(j2,1) / 100 + 240) < 0) (cursor2->location->y) = 0;
                        else (cursor2->location->y) = SDL_JoystickGetAxis(j2,1) / 100 + 240;
                        if((SDL_JoystickGetAxis(j2,0) / 100 + 340) > 640) (cursor2->location->x) = 640;
                        if((SDL_JoystickGetAxis(j2,1) / 100 + 240) > 480) (cursor2->location->y) = 480;
						if(contains(&target->location, cursor2->location->x, cursor2->location->y) && canCount2){
                        tempsToucherCible = tempsActuel - tempsPrecedent;//Pour calculer le temps
                        tempsPrecedent = tempsActuel; // Le temps "actuel" devient le temps "precedent" pour nos futurs calculs de temps

						game->player2Score = game->player2Score + calcul_points (game->level,tempsToucherCible, nbClic);

							if(incPoints(game,true)){
								explode(target, screen, game, cursor1, cursor2, display, thm, 4, 5); // doit etre appelée ici avant les menus
								char winStr[50];
								sprintf(winStr,"( avec %d points contre %d points )", game->player2PointsNumber, game->pointsNumber);
							    genRetCode=showGenericMenu(game, screen, display, cursor, &menuThm, "Niveau terminé !"," Gagnant : joueur 2 ", winStr);
								printf("Niveau terminé ! Gagnant: joueur 2 avec %d points contre %d pour le joueur 1\n", game->player2PointsNumber, game->pointsNumber);
								if(genRetCode==GENERIC_MENU_QUIT) gameState|=GAME_EXIT_QUIT;
								else gameState|=GAME_EXIT_STANDARD;
								waitSyncTime=false;
							}else{
								explode(target, screen, game, cursor1, cursor2, display, thm, 4, 5);
								printf("Joueur 2 marque un point :%d\nNouvelle vitesse : %f\n", game->player2PointsNumber, game->speed);
								theta = ((double)(rand())/RAND_MAX)*(2 * M_PI) - M_PI;
								x = rand()%(screen->window->w - target->surface->w);
								y = rand()%(screen->window->h - target->surface->h);
								waitSyncTime=false;
							}
							sprintf(score2_Str, "Joueur 2 : %d / %d", game->player2PointsNumber ,game->pointsLimit);
							sprintf(point2_Str, "Score J2 : %d", game->player2Score);
							SDL_FreeSurface(score2_Surface);
							SDL_FreeSurface(point2_Surface);
							score2_Surface=TTF_RenderText_Solid(mainFont, score2_Str, color);
							point2_Surface=TTF_RenderText_Solid(mainFont, point2_Str, color);
			    		nbClic=0; //Reinitialise le nombre de clics a chaque fois que le carré explose
			    	}
						else
						{
							nbClic++; //Compteur du nombre de clics
						}
						canCount2=false;
					}
					else if (SDL_JoystickGetButton(j1,2) == 1) {
						if((SDL_JoystickGetAxis(j1,0) / 100 + 300) < 0) (cursor1->location->x) = 0;
                        else (cursor1->location->x) = SDL_JoystickGetAxis(j1,0) / 100 + 300;
                        if((SDL_JoystickGetAxis(j1,1) / 100 + 240) < 0) (cursor1->location->y) = 0;
                        else (cursor1->location->y) = SDL_JoystickGetAxis(j1,1) / 100 + 240;
                        if((SDL_JoystickGetAxis(j1,0) / 100 + 300) > 640) (cursor1->location->x) = 640;
                        if((SDL_JoystickGetAxis(j1,1) / 100 + 240) > 480) (cursor1->location->y) = 480;
						if(contains(&target->location, cursor1->location->x, cursor1->location->y) && canCount1){
                       tempsToucherCible = tempsActuel - tempsPrecedent;//Pour calculer le temps
                        tempsPrecedent = tempsActuel; // Le temps "actuel" devient le temps "precedent" pour nos futurs calculs de temps

						game->score = game->score + calcul_points (game->level,tempsToucherCible, nbClic);


							if(incPoints(game,false)){
								explode(target, screen, game, cursor1, cursor2, display, thm, 4, 5);
								char winStr[50];
								sprintf(winStr,"( avec %d points contre %d points )", game->pointsNumber, game->player2PointsNumber);
								genRetCode=showGenericMenu(game, screen, display, cursor, &menuThm, "Niveau terminé !"," Gagnant : joueur 1", winStr);
								printf("Niveau terminé ! Gagnant: joueur 1 avec %d points contre %d pour le joueur 2\n", game->pointsNumber, game->player2PointsNumber);
								if(genRetCode==GENERIC_MENU_QUIT) gameState|=GAME_EXIT_QUIT;
								else gameState|=GAME_EXIT_STANDARD;
								waitSyncTime=false;
							}else{
								explode(target, screen, game, cursor1, cursor2, display, thm, 4, 5);
								printf("Joueur 1 marque un point :%d\nNouvelle vitesse : %f\n", game->pointsNumber, game->speed);
								theta = ((double)(rand())/RAND_MAX)*(2 * M_PI) - M_PI;
								x = rand()%(screen->window->w - target->surface->w);
								y = rand()%(screen->window->h - target->surface->h);
								waitSyncTime=false;
							}
							sprintf(score1_Str, "Joueur 1 : %d / %d", game->pointsNumber ,game->pointsLimit);
							sprintf(point1_Str, "Score J1 : %d", game->score);
							SDL_FreeSurface(point1_Surface);
							SDL_FreeSurface(score1_Surface);
							score1_Surface=TTF_RenderText_Solid(mainFont, score1_Str, color);
							point1_Surface=TTF_RenderText_Solid(mainFont, point1_Str, color);
                            nbClic=0; //Reinitialise le nombre de clics a chaque fois que le carré explose
			    	}
						else
						{
							nbClic++; //Compteur du nombre de clics
						}
						canCount1=false;
					}
				break ;
				case SDL_JOYBUTTONUP:
					canCount1=true;
					canCount2=true;
					break;
				default :
					break;
		    	}
	    		}
				//-----------------------------------------------------------------------------------------------
				//déplacement curseur 2
				//-----------------------------------------------------------------------------------------------
				//Déplacement vertical
				if(cur2Direction & 0x1){
					if(!cur2CountY){
							cursor2->location->y-=1;
							if(cursor2->location->y < 0) cursor2->location->y = 0;
						}
					if((++cur2CountY)>=game->player2KBSensitivity) cur2CountY=0;
				}else if(cur2Direction & 0x2){
					if(!cur2CountY){
						cursor2->location->y+=1;
						if(cursor2->location->y >= screen->window->h) cursor2->location->y = screen->window->h-1;
					}
					if((++cur2CountY)>=game->player2KBSensitivity) cur2CountY=0;
				}
				//Déplacement horizontal
				if(cur2Direction & 0x4){
					if(!cur2CountX){
						cursor2->location->x+=1;
						if(cursor2->location->x >= screen->window->w)	cursor2->location->x = screen->window->w-1;
					}
					if((++cur2CountX)>=game->player2KBSensitivity) cur2CountX=0;
				}else if(cur2Direction & 0x8){
					if(!cur2CountX){
						cursor2->location->x-=1;
						if(cursor2->location->x < 0) cursor2->location->x = 0;
					}
					if((++cur2CountX)>=game->player2KBSensitivity) cur2CountX=0;
				}


		}while(waitSyncTime && (SDL_GetTicks() < nextTime));
	} // Fin boucle principale

	SDL_FreeSurface(level_Surface);
	SDL_FreeSurface(score1_Surface);
	SDL_FreeSurface(score2_Surface);
	SDL_FreeSurface(point1_Surface);
	SDL_FreeSurface(point2_Surface);

	if(game->commandes == 2) {
        SDL_JoystickClose(j1);
        SDL_JoystickClose(j2);
	}

	TTF_CloseFont(mainFont);
	return gameState; // On retourne le code de sortie
}



//***************************************************************************
//                            	play
//    	Lance une nouvelle partie selon les paramètres de 'game'
//	sur la "fenetre" 'screen' avec les paramètres d'affichage 'display'
//***************************************************************************
GameExitCode play(GameParameters* game, DisplayParameters* display, Screen* screen, Cursor* cursor, Target* target, Theme* thm, char* levelName){
	GameExitCode exitCode;
	GenericMenuCode genRetCode;
	if(game->twoPlayers){
		genRetCode=showGenericMenu(game, screen, display, cursor, thm, "Squareshot", levelName, "Mode 2 joueurs");
		if(genRetCode==GENERIC_MENU_QUIT) exitCode=GAME_EXIT_QUIT;
		else exitCode=play2(game, display, screen, cursor, target, thm);
	}else{
		genRetCode=showGenericMenu(game, screen, display, cursor, thm, "Squareshot", levelName, "Mode solo");
		if(genRetCode==GENERIC_MENU_QUIT) exitCode=GAME_EXIT_QUIT;
		else exitCode=play1(game, display, screen, cursor, target, thm);
	}
	return exitCode;
}
#endif
