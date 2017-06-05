/**
* # pauseMenu.c
*/

#ifndef PAUSE_MENU
#define PAUSE_MENU

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


typedef enum {
	PAUSE_MENU_NONE=0,
	PAUSE_MENU_SETTINGS=1,
	PAUSE_MENU_BACK=2,
	PAUSE_MENU_EXIT=3,
	PAUSE_MENU_QUIT=4
}PauseMenuCode;


PauseMenuCode showPauseMenu(Screen* screen, DisplayParameters* display, Cursor* cursor, Theme* thm, int levelNb, GameParameters* game){

	SDL_Event event;
//Pour la manette
    if ( SDL_Init( SDL_INIT_JOYSTICK ) < 0 )
    return EXIT_FAILURE;
    SDL_Joystick *j1;
    j1 = SDL_JoystickOpen(0);
    SDL_JoystickEventState(SDL_ENABLE);

	bool canClick=true;
	PauseMenuCode exitCode=PAUSE_MENU_NONE;
	setThemeBackground(thm, screen->background);
	SDL_PollEvent(&event);
	if(game->commandes == 1)
        SDL_GetMouseState(&(cursor->location->x), &(cursor->location->y));
    else {
        (cursor->location->x) = 320;
        (cursor->location->y) = 120;
    }

	SDL_Color color= {thm->foreColor->R, thm->foreColor->G, thm->foreColor->B};
	SDL_Color titleColor= {200, 32, 64};
	TTF_Font* mainFont = TTF_OpenFont(thm->fontPath, 24);
	TTF_Font* titleFont = TTF_OpenFont(thm->fontPath, 32);

	char title[strlen("PAUSE - Niveau 9999")+1];
	sprintf(title, "PAUSE - Niveau %d", levelNb);
//Affichage du menu pause
	SDL_Surface* title_Surface=TTF_RenderText_Solid(titleFont, title, titleColor);
	SDL_Surface* settings_Surface=TTF_RenderText_Solid(mainFont, "Paramètres", color);
	SDL_Surface* back_Surface=TTF_RenderText_Solid(mainFont, "Retour au jeu", color);
	SDL_Surface* exit_Surface=TTF_RenderText_Solid(mainFont, "Abandonner", color);
//Création des rectangles de l'affichage du menu
	SDL_Rect title_Rect={(screen->window->w - title_Surface->w)/2, (screen->window->h /9) ,0 , 0};
	SDL_Rect settings_Rect={(screen->window->w - settings_Surface->w)/2, (screen->window->h /9) * 3,0,0};
	SDL_Rect back_Rect={(screen->window->w - back_Surface->w)/2, (screen->window->h /9) * 4 ,0,0};
	SDL_Rect exit_Rect={(screen->window->w - exit_Surface->w)/2, (screen->window->h /9) * 5 ,0,0};


	while(exitCode==PAUSE_MENU_NONE){

		SDL_BlitSurface(screen->background, NULL, screen->window, NULL);
		SDL_BlitSurface(title_Surface, NULL, screen->window, &title_Rect);
		SDL_BlitSurface(settings_Surface, NULL, screen->window, &settings_Rect);
		SDL_BlitSurface(back_Surface, NULL, screen->window, &back_Rect);
		SDL_BlitSurface(exit_Surface, NULL, screen->window, &exit_Rect);
		drawCursor(cursor, screen->window);
		SDL_Flip(screen->window);

		SDL_WaitEvent(&event);

//Pour la souris
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
						exitCode=PAUSE_MENU_SETTINGS;
					}else if(containsP(&back_Rect, cursor->location)){
						exitCode=PAUSE_MENU_BACK;
					}else if(containsP(&exit_Rect, cursor->location)){
						exitCode=PAUSE_MENU_EXIT;
					}
				}
				break;
			case SDL_MOUSEBUTTONUP:
				canClick=true;
				break;
			case SDL_QUIT:
				exitCode=PAUSE_MENU_QUIT;
				break;
			case SDL_KEYDOWN:
				if(event.key.keysym.sym==SDLK_ESCAPE) exitCode=PAUSE_MENU_BACK;
				else if(event.key.keysym.sym==SDLK_p) exitCode=PAUSE_MENU_SETTINGS;
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
			case SDL_JOYBUTTONDOWN:
				if(canClick){
					canClick=false;
					(cursor->location->x) = SDL_JoystickGetAxis(j1,0) / 100 + 320;
					(cursor->location->y) = SDL_JoystickGetAxis(j1,1) / 100 + 120;
					if(containsP(&settings_Rect, cursor->location)){
						exitCode=PAUSE_MENU_SETTINGS;
					}else if(containsP(&back_Rect, cursor->location)){
						exitCode=PAUSE_MENU_BACK;
					}else if(containsP(&exit_Rect, cursor->location)){
						exitCode=PAUSE_MENU_EXIT;
					}
				}
				break;
			case SDL_JOYBUTTONUP:
				canClick=true;
				break;
			case SDL_QUIT:
				exitCode=PAUSE_MENU_QUIT;
				break;
			case SDL_KEYDOWN:
				if(event.key.keysym.sym==SDLK_ESCAPE) exitCode=PAUSE_MENU_BACK;
				else if(event.key.keysym.sym==SDLK_p) exitCode=PAUSE_MENU_SETTINGS;
				break;
			default: break;
		}
		}
	}
//Libération de l'affichage
	SDL_FreeSurface(title_Surface);
	SDL_FreeSurface(settings_Surface);
	SDL_FreeSurface(back_Surface);
	SDL_FreeSurface(exit_Surface);
	TTF_CloseFont(mainFont);
	TTF_CloseFont(titleFont);
	return exitCode;
}


#endif
