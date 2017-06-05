/**
* # mainMenu.c
*/

#ifndef MAIN_MENU
#define MAIN_MENU

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
#include "files.c"
#include "point.c"
#include "settingsMenu.c"
#include "genericMenu.c"
#include "highScoresMenu.c"
#include "game.c"


typedef enum {
	MAIN_MENU_NONE=-1,
	MAIN_MENU_EXIT=0,
	MAIN_MENU_1_PLAYER=1,
	MAIN_MENU_2_PLAYERS=2,
	MAIN_MENU_SETTINGS=3,
	MAIN_MENU_HIGH_SCORES=4
}MainMenuCode;


MainMenuCode showMainMenu(GameParameters* game, Screen* screen, DisplayParameters* display, Cursor* cursor, Theme* thm, char* msg){

	SDL_Event event;
//Si joystick
	if ( SDL_Init( SDL_INIT_JOYSTICK ) < 0 )
        return EXIT_FAILURE;
    SDL_Joystick *j1;
    j1 = SDL_JoystickOpen(0);
    SDL_JoystickEventState(SDL_ENABLE);

	bool canClick=true;
	MainMenuCode exitCode=MAIN_MENU_NONE;
	setThemeBackground(thm, screen->background);
	SDL_PollEvent(&event);

	if(game->commandes == 1) {
        SDL_GetMouseState(&(cursor->location->x), &(cursor->location->y));
	}
	else {
        (cursor->location->x) = 320;
        (cursor->location->y) = 120;
	}

	SDL_Color color= {thm->foreColor->R, thm->foreColor->G, thm->foreColor->B};
	SDL_Color titleColor= {200, 32, 64};
	TTF_Font* mainFont = TTF_OpenFont(thm->fontPath, 24);
	TTF_Font* titleFont = TTF_OpenFont(thm->fontPath, 32);

//Messages à afficher pour le menu
	SDL_Surface* title_Surface=TTF_RenderText_Solid(titleFont, "[ SQUARESHOT ]", titleColor);
	SDL_Surface* oneP_Surface=TTF_RenderText_Solid(mainFont, "1 Joueur", color);
	SDL_Surface* twoP_Surface=TTF_RenderText_Solid(mainFont, "2 Joueurs", color);
	SDL_Surface* high_Scores_Surface=TTF_RenderText_Solid(mainFont, "High Scores", color);
	SDL_Surface* settings_Surface=TTF_RenderText_Solid(mainFont, "Paramètres", color);
	SDL_Surface* exit_Surface=TTF_RenderText_Solid(mainFont, "Quitter", color);
	SDL_Surface* msg_Surface=NULL;

//Création des rectangles accueillant les sections du programme
	SDL_Rect title_Rect={(screen->window->w - title_Surface->w)/2, (screen->window->h /9) ,0 , 0};
	SDL_Rect oneP_Rect={(screen->window->w - oneP_Surface->w)/2, (screen->window->h /9) * 3 ,0,0};
	SDL_Rect twoP_Rect={(screen->window->w - twoP_Surface->w)/2, (screen->window->h /9) * 4 ,0,0};
    SDL_Rect high_Scores_Rect={(screen->window->w - high_Scores_Surface->w)/2, (screen->window->h /9) * 5, 0 ,0};
	SDL_Rect settings_Rect={(screen->window->w - settings_Surface->w)/2, (screen->window->h /9) * 6,0,0};
	SDL_Rect exit_Rect={(screen->window->w - exit_Surface->w)/2, (screen->window->h /9) * 7 ,0,0};
	SDL_Rect msg_Rect;

	if(msg!=NULL){
		SDL_Color msg_color= {200, 32, 64};
		msg_Surface=TTF_RenderText_Solid(mainFont, msg, msg_color);
		msg_Rect.x=(screen->window->w - msg_Surface->w)/2;
		msg_Rect.y=(screen->window->h /9) * 7;
	}

	while(exitCode==MAIN_MENU_NONE){

		SDL_BlitSurface(screen->background, NULL, screen->window, NULL);
		SDL_BlitSurface(title_Surface, NULL, screen->window, &title_Rect);
		SDL_BlitSurface(oneP_Surface, NULL, screen->window, &oneP_Rect);
        SDL_BlitSurface(high_Scores_Surface, NULL, screen->window, &high_Scores_Rect);
		SDL_BlitSurface(twoP_Surface, NULL, screen->window, &twoP_Rect);
		SDL_BlitSurface(settings_Surface, NULL, screen->window, &settings_Rect);
		SDL_BlitSurface(exit_Surface, NULL, screen->window, &exit_Rect);
		if(msg!=NULL) SDL_BlitSurface(msg_Surface, NULL, screen->window, &msg_Rect);
		drawCursor(cursor, screen->window);
		SDL_Flip(screen->window);

		SDL_WaitEvent(&event);
//Pour que quand on clic avec la souris, on vérifie si la souris est sur un des rectangles du menu
		if(game->commandes == 1) {
            switch(event.type){
			case SDL_MOUSEMOTION:
				SDL_GetMouseState(&(cursor->location->x), &(cursor->location->y));
				break;
			case SDL_MOUSEBUTTONDOWN:
				if(canClick){
					canClick=false;
					SDL_GetMouseState(&(cursor->location->x), &(cursor->location->y));
					if(containsP(&settings_Rect, cursor->location)){
						exitCode=MAIN_MENU_SETTINGS;
					}else if(containsP(&oneP_Rect, cursor->location)){
						exitCode=MAIN_MENU_1_PLAYER;
					}else if(containsP(&twoP_Rect, cursor->location)){
						exitCode=MAIN_MENU_2_PLAYERS;
					}else if(containsP(&high_Scores_Rect, cursor->location)){
                        exitCode=MAIN_MENU_HIGH_SCORES;
					}else if(containsP(&exit_Rect, cursor->location)){
						exitCode=MAIN_MENU_EXIT;
					}
				}
				break;
			case SDL_MOUSEBUTTONUP:
				canClick=true;
				break;
			case SDL_QUIT:
				exitCode=MAIN_MENU_EXIT;
				break;
			case SDL_KEYDOWN:
				if(event.key.keysym.sym==SDLK_ESCAPE) exitCode=MAIN_MENU_EXIT;
				else if(event.key.keysym.sym==SDLK_p) exitCode=MAIN_MENU_SETTINGS;
                else if(event.key.keysym.sym==SDLK_KP3) exitCode=MAIN_MENU_HIGH_SCORES;
				else if(event.key.keysym.sym==SDLK_KP1) exitCode=MAIN_MENU_1_PLAYER;
				else if(event.key.keysym.sym==SDLK_KP2) exitCode=MAIN_MENU_2_PLAYERS;
				break;
			default: break;
		}
		}
//Pour le joystick
		else {
            switch(event.type){
			case SDL_JOYAXISMOTION:
				if((SDL_JoystickGetAxis(j1,0) / 100 + 320) < 0) (cursor->location->x) = 0;
                else (cursor->location->x) = SDL_JoystickGetAxis(j1,0) / 100 + 320;
                if((SDL_JoystickGetAxis(j1,1) / 100 + 120) < 0) (cursor->location->y) = 0;
                else (cursor->location->y) = SDL_JoystickGetAxis(j1,1) / 100 + 120;
                if((SDL_JoystickGetAxis(j1,0) / 100 + 320) > 640) (cursor->location->x) = 640;
                if((SDL_JoystickGetAxis(j1,1) / 100 + 120) > 480) (cursor->location->y) = 480;
				break;

//Si on appuie sur le bouton, on vérifie sa position comme avec la souris
			case SDL_JOYBUTTONDOWN:
				if(canClick){
					canClick=false;
					(cursor->location->x) = SDL_JoystickGetAxis(j1, 0) / 100 + 320;
                    (cursor->location->y) = SDL_JoystickGetAxis(j1, 1) / 100 + 120;
					if(containsP(&settings_Rect, cursor->location)){
						exitCode=MAIN_MENU_SETTINGS;
					}else if(containsP(&oneP_Rect, cursor->location)){
						exitCode=MAIN_MENU_1_PLAYER;
					}else if(containsP(&twoP_Rect, cursor->location)){
						exitCode=MAIN_MENU_2_PLAYERS;
                    }else if(containsP(&high_Scores_Rect, cursor->location)){//RAJOUT ICI
                        exitCode=MAIN_MENU_HIGH_SCORES;//Et ici
					}else if(containsP(&exit_Rect, cursor->location)){
						exitCode=MAIN_MENU_EXIT;
					}
				}
				break;
			case SDL_JOYBUTTONUP:
				canClick=true;
				break;
			case SDL_QUIT:
				exitCode=MAIN_MENU_EXIT;
				break;
			case SDL_KEYDOWN:
				if(event.key.keysym.sym==SDLK_ESCAPE) exitCode=MAIN_MENU_EXIT;
				else if(event.key.keysym.sym==SDLK_p) exitCode=MAIN_MENU_SETTINGS;
				else if(event.key.keysym.sym==SDLK_KP1) exitCode=MAIN_MENU_1_PLAYER;
				else if(event.key.keysym.sym==SDLK_KP2) exitCode=MAIN_MENU_2_PLAYERS;
				break;
			default: break;
		}
		}

	}

    SDL_FreeSurface(title_Surface);
	SDL_FreeSurface(settings_Surface);
	SDL_FreeSurface(oneP_Surface);
	SDL_FreeSurface(twoP_Surface);
	SDL_FreeSurface(high_Scores_Surface);
	SDL_FreeSurface(exit_Surface);
	if(game->commandes == 2) SDL_JoystickClose(j1); // modif
	if(msg!=NULL)SDL_FreeSurface(msg_Surface);
	TTF_CloseFont(mainFont);
	TTF_CloseFont(titleFont);
	return exitCode;
}


#endif
