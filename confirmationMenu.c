/**
* # confirmationMenu.c
*
*/

#ifndef CONFIRMATION_MENU
#define CONFIRMATION_MENU

#include "gestionPoints.c"
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_TTF.h>

typedef enum {
	CONFIRMATION_MENU_NONE=0,
	CONFIRMATION_MENU_EXIT=3,
	CONFIRMATION_MENU_QUIT=2
}ConfirmationMenuCode;


ConfirmationMenuCode showConfirmationMenu(Screen* screen, DisplayParameters* display, Cursor* cursor, Theme* thm, GameParameters* game){
	SDL_Event event;
	bool canClick=true;
	ConfirmationMenuCode exitCode=CONFIRMATION_MENU_NONE;
	setThemeBackground(thm, screen->background);
	SDL_PollEvent(&event);
//Pour l'utilisation du joystick
    if ( SDL_Init( SDL_INIT_JOYSTICK ) < 0 )
    return EXIT_FAILURE;
    SDL_Joystick *j1;
    j1 = SDL_JoystickOpen(0);
    SDL_JoystickEventState(SDL_ENABLE);
	if(game->commandes == 1)
//Ou de la souris
        {
        SDL_GetMouseState(&(cursor->location->x), &(cursor->location->y));
	}
	else {
        (cursor->location->x) = 320;
        (cursor->location->y) = 240;
	}

	SDL_Color color= {thm->foreColor->R, thm->foreColor->G, thm->foreColor->B};
	SDL_Color titleColor= {200, 32, 64};
	TTF_Font* mainFont = TTF_OpenFont(thm->fontPath, 16);
	TTF_Font* questionFont = TTF_OpenFont(thm->fontPath, 24);
	TTF_Font* titleFont = TTF_OpenFont(thm->fontPath, 32);

//Affichage
	SDL_Surface* title_Surface=TTF_RenderText_Solid(titleFont, "Confirmation", titleColor);
	SDL_Surface* question_Surface=TTF_RenderText_Solid(questionFont, "Reinitialiser les scores ?", color);
	SDL_Surface* oui_Surface=TTF_RenderText_Solid(mainFont, "Oui", color);
	SDL_Surface* non_Surface=TTF_RenderText_Solid(mainFont, "Non", color);

	SDL_Rect title_Rect={(screen->window->w - title_Surface->w)/2, (screen->window->h /15) ,0 , 0};
	SDL_Rect question_Rect={(screen->window->w - question_Surface->w)/2, (screen->window->h /16) * 3,0,0};
	SDL_Rect oui_Rect={(screen->window->w - oui_Surface->w)/2, (screen->window->h /15) * 4 ,0,0};
	SDL_Rect non_Rect={(screen->window->w - non_Surface->w)/2, (screen->window->h /15) * 5 ,0,0};

	while(exitCode==CONFIRMATION_MENU_NONE){

		SDL_BlitSurface(screen->background, NULL, screen->window, NULL);
		SDL_BlitSurface(title_Surface, NULL, screen->window, &title_Rect);
		SDL_BlitSurface(question_Surface, NULL, screen->window, &question_Rect);
		SDL_BlitSurface(oui_Surface, NULL, screen->window, &oui_Rect);
		SDL_BlitSurface(non_Surface, NULL, screen->window, &non_Rect);
		drawCursor(cursor, screen->window);
		SDL_Flip(screen->window);

		SDL_WaitEvent(&event);
//Gestion de la souris
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
                    if(containsP(&oui_Rect, cursor->location)){
						exitCode=CONFIRMATION_MENU_EXIT;
						init_score();
				}else if(containsP(&non_Rect, cursor->location))
				{
				   exitCode=CONFIRMATION_MENU_EXIT;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				canClick=true;
				break;
			case SDL_QUIT:
				exitCode=CONFIRMATION_MENU_QUIT;
				break;
			case SDL_KEYDOWN:
				if(event.key.keysym.sym==SDLK_ESCAPE){ exitCode=CONFIRMATION_MENU_EXIT;}
				break;
			default: break;
        }
		}
//gestion du joystick
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
                    if(containsP(&oui_Rect, cursor->location)){
						exitCode=CONFIRMATION_MENU_EXIT;
						init_score();
				}else if(containsP(&non_Rect, cursor->location))
				{
				   exitCode=CONFIRMATION_MENU_EXIT;
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
	SDL_FreeSurface(question_Surface);
	SDL_FreeSurface(oui_Surface);
	SDL_FreeSurface(non_Surface);
	TTF_CloseFont(mainFont);
	TTF_CloseFont(titleFont);
	TTF_CloseFont(questionFont);
	return exitCode;
}


#endif
