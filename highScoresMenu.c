/**
* # highScoresMenu.c
*/

#ifndef HIGH_SCORES_MENU
#define HIGH_SCORES_MENU

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "functions.c"
#include "display.c"
#include "cursor.c"
#include "themes.c"
#include "colors.c"
#include "images.c"
#include "gestionPoints.c"
#include "files.c"
#include "point.c"

/*
Fichier du menu d'affichage des high scores.
*/

typedef enum {
	HIGH_SCORES_MENU_NONE=0,
	HIGH_SCORES_MENU_RELOAD=1,
	HIGH_SCORES_MENU_EXIT=4,
	HIGH_SCORES_MENU_QUIT=3,
	HIGH_SCORES_MENU_CONFIRMATION=2
}HighScoresMenuCode;


HighScoresMenuCode showHighScoresMenu(Screen* screen, DisplayParameters* display, Cursor* cursor, Theme* thm, GameParameters* game){
	SDL_Event event;
	bool canClick=true;
	int points_joueur=0;
	int pseudo_joueur[16];
	HighScoresMenuCode exitCode=HIGH_SCORES_MENU_NONE;
	setThemeBackground(thm, screen->background);
	SDL_PollEvent(&event);
    if ( SDL_Init( SDL_INIT_JOYSTICK ) < 0 )
    return EXIT_FAILURE;
    SDL_Joystick *j1;
    j1 = SDL_JoystickOpen(0);
    SDL_JoystickEventState(SDL_ENABLE);
	if(game->commandes == 1) {
        SDL_GetMouseState(&(cursor->location->x), &(cursor->location->y));
	}
	else {
        (cursor->location->x) = 320;
        (cursor->location->y) = 240;
	}

	SDL_Color color= {thm->foreColor->R, thm->foreColor->G, thm->foreColor->B};
	SDL_Color titleColor= {200, 32, 64};
	TTF_Font* mainFont = TTF_OpenFont(thm->fontPath, 24);
	TTF_Font* titleFont = TTF_OpenFont(thm->fontPath, 32);
    TTF_Font* playerFont = TTF_OpenFont(thm->fontPath, 16);

    char ligne_Str[25];//Création de la chaîne de caractère de la ligne de score
//Affichage en haut de la fenêtres
	SDL_Surface* title_Surface=TTF_RenderText_Solid(titleFont, "High Scores", titleColor);
	SDL_Surface* under_Title_Surface=TTF_RenderText_Solid(mainFont, "Top 10 des joueurs", color);
//Affichage des 10 scores et pseudo
    lire_score(1,&points_joueur,pseudo_joueur);
    sprintf(ligne_Str,"%s   %d",pseudo_joueur, points_joueur);

	SDL_Surface* player_Surface=TTF_RenderText_Solid(playerFont, ligne_Str, color);

    lire_score(2,&points_joueur,pseudo_joueur);
    sprintf(ligne_Str,"%s   %d",pseudo_joueur, points_joueur);

    SDL_Surface* player1_Surface=TTF_RenderText_Solid(playerFont, ligne_Str, color);

    lire_score(3,&points_joueur,pseudo_joueur);
    sprintf(ligne_Str,"%s   %d",pseudo_joueur, points_joueur);

	SDL_Surface* player2_Surface=TTF_RenderText_Solid(playerFont, ligne_Str, color);

    lire_score(4,&points_joueur,pseudo_joueur);
    sprintf(ligne_Str,"%s   %d",pseudo_joueur, points_joueur);

	SDL_Surface* player3_Surface=TTF_RenderText_Solid(playerFont, ligne_Str, color);

    lire_score(5,&points_joueur,pseudo_joueur);
    sprintf(ligne_Str,"%s   %d",pseudo_joueur, points_joueur);

	SDL_Surface* player4_Surface=TTF_RenderText_Solid(playerFont, ligne_Str, color);

    lire_score(6,&points_joueur,pseudo_joueur);
    sprintf(ligne_Str,"%s   %d",pseudo_joueur, points_joueur);

	SDL_Surface* player5_Surface=TTF_RenderText_Solid(playerFont, ligne_Str, color);

    lire_score(7,&points_joueur,pseudo_joueur);
    sprintf(ligne_Str,"%s   %d",pseudo_joueur, points_joueur);

	SDL_Surface* player6_Surface=TTF_RenderText_Solid(playerFont, ligne_Str, color);

    lire_score(8,&points_joueur,pseudo_joueur);
    sprintf(ligne_Str,"%s   %d",pseudo_joueur, points_joueur);

	SDL_Surface* player7_Surface=TTF_RenderText_Solid(playerFont, ligne_Str, color);

    lire_score(9,&points_joueur,pseudo_joueur);
    sprintf(ligne_Str,"%s   %d",pseudo_joueur, points_joueur);

	SDL_Surface* player8_Surface=TTF_RenderText_Solid(playerFont, ligne_Str, color);

    lire_score(10,&points_joueur,pseudo_joueur);
    sprintf(ligne_Str,"%s   %d",pseudo_joueur, points_joueur);
//Affichage du bas de la fenêtre
	SDL_Surface* player9_Surface=TTF_RenderText_Solid(playerFont, ligne_Str, color);
	SDL_Surface* reinit_Surface=TTF_RenderText_Solid(playerFont, "Réinitialiser", color);
	SDL_Surface* back_Surface=TTF_RenderText_Solid(playerFont, "Retour", color);
//Création des rectangles de pseudo et score
	SDL_Rect title_Rect={(screen->window->w - title_Surface->w)/2, (screen->window->h /15) ,0 , 0};
	SDL_Rect under_Title_Rect={(screen->window->w - under_Title_Surface->w)/2, (screen->window->h /16) * 3,0,0};
	SDL_Rect player_Rect={(screen->window->w - player_Surface->w)/2, (screen->window->h /15) * 4 ,0,0};
	SDL_Rect player1_Rect={(screen->window->w - player1_Surface->w)/2, (screen->window->h /15) * 5 ,0,0};
	SDL_Rect player2_Rect={(screen->window->w - player2_Surface->w)/2, (screen->window->h /15) * 6 ,0,0};
	SDL_Rect player3_Rect={(screen->window->w - player3_Surface->w)/2, (screen->window->h /15) * 7 ,0,0};
	SDL_Rect player4_Rect={(screen->window->w - player4_Surface->w)/2, (screen->window->h /15) * 8 ,0,0};
	SDL_Rect player5_Rect={(screen->window->w - player5_Surface->w)/2, (screen->window->h /15) * 9 ,0,0};
    SDL_Rect player6_Rect={(screen->window->w - player6_Surface->w)/2, (screen->window->h /15) * 10 ,0,0};
	SDL_Rect player7_Rect={(screen->window->w - player7_Surface->w)/2, (screen->window->h /15) * 11 ,0,0};
	SDL_Rect player8_Rect={(screen->window->w - player8_Surface->w)/2, (screen->window->h /15) * 12 ,0,0};
	SDL_Rect player9_Rect={(screen->window->w - player9_Surface->w)/2, (screen->window->h /15) * 13 ,0,0};
	SDL_Rect back_Rect={(screen->window->w - back_Surface->w)/2, (screen->window->h /15) * 14, 0, 0};
	SDL_Rect reinit_Rect={(screen->window->w - back_Surface->w)/25, (screen->window->h /15) * 14,0,0};

	while(exitCode==HIGH_SCORES_MENU_NONE){

		SDL_BlitSurface(screen->background, NULL, screen->window, NULL);
		SDL_BlitSurface(title_Surface, NULL, screen->window, &title_Rect);
		SDL_BlitSurface(under_Title_Surface, NULL, screen->window, &under_Title_Rect);
		SDL_BlitSurface(player_Surface, NULL, screen->window, &player_Rect);
		SDL_BlitSurface(player1_Surface, NULL, screen->window, &player1_Rect);
		SDL_BlitSurface(player2_Surface, NULL, screen->window, &player2_Rect);
		SDL_BlitSurface(player3_Surface, NULL, screen->window, &player3_Rect);
		SDL_BlitSurface(player4_Surface, NULL, screen->window, &player4_Rect);
		SDL_BlitSurface(player5_Surface, NULL, screen->window, &player5_Rect);
		SDL_BlitSurface(player6_Surface, NULL, screen->window, &player6_Rect);
		SDL_BlitSurface(player7_Surface, NULL, screen->window, &player7_Rect);
		SDL_BlitSurface(player8_Surface, NULL, screen->window, &player8_Rect);
		SDL_BlitSurface(player9_Surface, NULL, screen->window, &player9_Rect);
		SDL_BlitSurface(back_Surface, NULL, screen->window, &back_Rect);
		SDL_BlitSurface(reinit_Surface,NULL,screen->window, &reinit_Rect);
		drawCursor(cursor, screen->window);
		SDL_Flip(screen->window);

		SDL_WaitEvent(&event);

//Détection des movements et du boutons de la souris
		if(game->commandes == 1) {
            switch(event.type){
			case SDL_MOUSEMOTION:
				SDL_GetMouseState(&(cursor->location->x), &(cursor->location->y));
				break;
			case SDL_MOUSEBUTTONDOWN:
				if(canClick){
					canClick=false;
					SDL_GetMouseState(&(cursor->location->x), &(cursor->location->y));
					}
                    if(containsP(&back_Rect, cursor->location)){
						exitCode=HIGH_SCORES_MENU_EXIT;
				}else if(containsP(&reinit_Rect, cursor->location))
				{
				    exitCode=HIGH_SCORES_MENU_CONFIRMATION;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				canClick=true;
				break;
			case SDL_QUIT:
				exitCode=HIGH_SCORES_MENU_QUIT;
				break;
			case SDL_KEYDOWN:
				if(event.key.keysym.sym==SDLK_ESCAPE){ exitCode=HIGH_SCORES_MENU_EXIT;}
				break;
			default: break;
        }
		}
//Idem pour le joystick
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
			case SDL_JOYBUTTONDOWN:
				if(canClick){
					canClick=false;
					if((SDL_JoystickGetAxis(j1,0) / 100 + 320) < 0) (cursor->location->x) = 0;
                    else (cursor->location->x) = SDL_JoystickGetAxis(j1,0) / 100 + 320;
                    if((SDL_JoystickGetAxis(j1,1) / 100 + 240) < 0) (cursor->location->y) = 0;
                    else (cursor->location->y) = SDL_JoystickGetAxis(j1,1) / 100 + 240;
                    if((SDL_JoystickGetAxis(j1,0) / 100 + 320) > 640) (cursor->location->x) = 640;
                    if((SDL_JoystickGetAxis(j1,1) / 100 + 240) > 480) (cursor->location->y) = 480;
					}
                    if(containsP(&back_Rect, cursor->location)){
						exitCode=HIGH_SCORES_MENU_EXIT;
				}else if(containsP(&reinit_Rect, cursor->location))
				{
				    exitCode=HIGH_SCORES_MENU_CONFIRMATION;
				}
				break;
			case SDL_JOYBUTTONUP:
				canClick=true;
                break;
			default: break;
        }
		}
	}

//Fin de tout les affichages
	SDL_FreeSurface(title_Surface);
	SDL_FreeSurface(under_Title_Surface);
	SDL_FreeSurface(player_Surface);
	SDL_FreeSurface(player1_Surface);
	SDL_FreeSurface(player2_Surface);
	SDL_FreeSurface(player3_Surface);
	SDL_FreeSurface(player4_Surface);
	SDL_FreeSurface(player5_Surface);
	SDL_FreeSurface(player6_Surface);
	SDL_FreeSurface(player7_Surface);
	SDL_FreeSurface(player8_Surface);
	SDL_FreeSurface(player9_Surface);
	SDL_FreeSurface(back_Surface);
	SDL_FreeSurface(reinit_Surface);
	TTF_CloseFont(mainFont);
	TTF_CloseFont(titleFont);
	TTF_CloseFont(playerFont);
	return exitCode;
}


#endif
