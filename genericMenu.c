/**
* # genericMenu.c
*/

#ifndef GENERIC_MENU
#define GENERIC_MENU

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
#include "point.c"
#include "game.c"
#include "gestionPoints.c"

typedef enum {
	GENERIC_MENU_NONE=0,
	GENERIC_MENU_CONTINUE=1,
	GENERIC_MENU_QUIT=2
}GenericMenuCode;


GenericMenuCode showGenericMenu(GameParameters* game,Screen* screen, DisplayParameters* display, Cursor* cursor, Theme* thm, char* title, char* msg1, char* msg2){

	SDL_Event event;
	/*Ajout du mode manette*/
    if ( SDL_Init( SDL_INIT_JOYSTICK ) < 0 )
    return EXIT_FAILURE;
    SDL_Joystick *j1;
    j1 = SDL_JoystickOpen(0);
    SDL_JoystickEventState(SDL_ENABLE);
    /*fin ajout des manettes*/
	GenericMenuCode exitCode=GENERIC_MENU_NONE;
	setThemeBackground(thm, screen->background);
	SDL_PollEvent(&event);
	if(game->commandes == 1)
        SDL_GetMouseState(&(cursor->location->x), &(cursor->location->y));
    else {
        (cursor->location->x) = 320;
        (cursor->location->y) = 240;
    }

	SDL_Color color= {thm->foreColor->R, thm->foreColor->G, thm->foreColor->B};
	SDL_Color color2= {200, 32, 64};
	TTF_Font* littleFont  = TTF_OpenFont(thm->fontPath, 12);
	TTF_Font* mainFont = TTF_OpenFont(thm->fontPath, 16);
	TTF_Font* titleFont = TTF_OpenFont(thm->fontPath, 24);

	SDL_Surface *title_Surface=NULL, *msg1_Surface=NULL, *msg2_Surface=NULL;
	SDL_Surface* exit_Surface;
    // Affichage du texte selon le mode de commandes
	if(game->commandes == 1) {
        exit_Surface=TTF_RenderText_Solid(littleFont, "(Cliquez ou appuyez sur Entree pour continuer)", color);
	}
    else {
        exit_Surface=TTF_RenderText_Solid(littleFont, "(Appuyez sur un bouton pour continuer)", color);
    }


	SDL_Rect title_Rect, msg2_Rect, msg1_Rect;
	SDL_Rect exit_Rect={(screen->window->w - exit_Surface->w)/2, (screen->window->h /9) * 8 ,0,0};

	if(title!=NULL){
		title_Surface=TTF_RenderText_Solid(titleFont, title, color2);
		title_Rect.x=(screen->window->w - title_Surface->w)/2;
		title_Rect.y=(screen->window->h /9);
	}
	if(msg1!=NULL){
		msg1_Surface=TTF_RenderText_Solid(mainFont, msg1, color);
		msg1_Rect.x=(screen->window->w - msg1_Surface->w)/2;
		msg1_Rect.y=(screen->window->h /9)*3;
	}
	if(msg2!=NULL){
		msg2_Surface=TTF_RenderText_Solid(mainFont, msg2, color);
		msg2_Rect.x=(screen->window->w - msg2_Surface->w)/2;
		msg2_Rect.y=(screen->window->h /9)*5;
	}

	while(exitCode==GENERIC_MENU_NONE){

		SDL_BlitSurface(screen->background, NULL, screen->window, NULL);
		if(title!=NULL) SDL_BlitSurface(title_Surface, NULL, screen->window, &title_Rect);
		if(msg1!=NULL) SDL_BlitSurface(msg1_Surface, NULL, screen->window, &msg1_Rect);
		if(msg2!=NULL) SDL_BlitSurface(msg2_Surface, NULL, screen->window, &msg2_Rect);
		SDL_BlitSurface(exit_Surface, NULL, screen->window, &exit_Rect);
		drawCursor(cursor, screen->window);
		SDL_Flip(screen->window);

		SDL_WaitEvent(&event);
//Pour l'utilisation de la souris
		if(game->commandes == 1) {
            switch(event.type){
			case SDL_MOUSEMOTION:
				SDL_GetMouseState(&(cursor->location->x), &(cursor->location->y));
				break;
			case SDL_MOUSEBUTTONDOWN:
				exitCode=GENERIC_MENU_CONTINUE;
				break;
			case SDL_QUIT:
				exitCode=GENERIC_MENU_QUIT;
				break;
			case SDL_KEYDOWN:
				if(event.key.keysym.sym==SDLK_RETURN) exitCode=GENERIC_MENU_CONTINUE;
				break;
			default: break;
		}
		}
//Pour l'utilisation de la manettes
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
				exitCode=GENERIC_MENU_CONTINUE;
				break;
			case SDL_QUIT:
				exitCode=GENERIC_MENU_QUIT;
				break;
			case SDL_KEYDOWN:
				if(event.key.keysym.sym==SDLK_RETURN) exitCode=GENERIC_MENU_CONTINUE;
				break;
			default: break;
		}
		}

	}


	if(title!=NULL)SDL_FreeSurface(title_Surface);
	if(msg1!=NULL)SDL_FreeSurface(msg1_Surface);
	if(msg2!=NULL)SDL_FreeSurface(msg2_Surface);
	if(game->commandes == 2) SDL_JoystickClose(j1); //Si on a utilisé les manettes on doit fermer la connection joystick
	SDL_FreeSurface(exit_Surface);
	TTF_CloseFont(mainFont);
	TTF_CloseFont(titleFont);
	TTF_CloseFont(littleFont);
	return exitCode;
}


#endif
