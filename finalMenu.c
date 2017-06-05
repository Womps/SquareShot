/**
* finalMenu.c
*
*Fichier contenant le menu affiché à la fin du jeu, afin de saisir le pseudo du joueur.
*
*/
#ifndef FINAL_MENU
#define FINAL_MENU
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>

typedef enum{
    FINAL_MENU_NONE=0,
	FINAL_MENU_CONTINUE=1,
	FINAL_MENU_QUIT=2
    }FinalMenuCode;


FinalMenuCode showFinalMenu(GameParameters* game,Screen* screen, DisplayParameters* display, Cursor* cursor, Theme* thm, char chaine[], int longeur){
       //fonction qui réalise la saisie du pseudo du joueur dans la chaine passé en paramètre
       //possibilité de changer les 1/0 par true/false (mais ajouter la librairie pour...)

	SDL_Event event;
    SDL_EnableUNICODE(1);
    SDL_EnableKeyRepeat(120,120);
	FinalMenuCode exitCode=FINAL_MENU_NONE;
	setThemeBackground(thm, screen->background);
	SDL_PollEvent(&event);
	SDL_GetMouseState(&(cursor->location->x), &(cursor->location->y));


	SDL_Color color= {thm->foreColor->R, thm->foreColor->G, thm->foreColor->B};
	SDL_Color titlecolor= {200, 32, 64};
	TTF_Font* mainFont = TTF_OpenFont(thm->fontPath, 24);
	TTF_Font* titleFont = TTF_OpenFont(thm->fontPath, 32);
    TTF_Font* littleFont = TTF_OpenFont(thm->fontPath, 16);
//Affichage
	SDL_Surface* title_Surface=TTF_RenderText_Solid(titleFont, "FIN", titlecolor);
	SDL_Surface* msg1_Surface=TTF_RenderText_Solid(mainFont, "Felicitations !", color);
	SDL_Surface* saisie_Surface=TTF_RenderText_Solid(mainFont, chaine, color);
	SDL_Surface* exit_Surface=TTF_RenderText_Solid(littleFont, "(Entrez votre pseudo et tapez Entrée)", color);

	SDL_Rect title_Rect={(screen->window->w - title_Surface->w)/2, (screen->window->h /9), 0, 0};
	SDL_Rect msg1_Rect={(screen->window->w - msg1_Surface->w)/2, (screen->window->h /9) * 3,0,0};
	SDL_Rect saisie_Rect={(screen->window->w - msg1_Surface->w)/2, (screen->window->h /9) * 5, 0,0};
	SDL_Rect exit_Rect={(screen->window->w - exit_Surface->w)/2, (screen->window->h /9) * 8 ,0,0};

    int i = 0;//indice de la chaine
    while (i<longeur)  //réinitialisation de la chaine au cas où...
    {
        chaine[i]=0;
        i++;
    }
    i = 0;
	while(exitCode==FINAL_MENU_NONE){

		SDL_BlitSurface(screen->background, NULL, screen->window, NULL);
		SDL_BlitSurface(title_Surface, NULL, screen->window, &title_Rect);
		SDL_BlitSurface(msg1_Surface, NULL, screen->window, &msg1_Rect);
		SDL_BlitSurface(saisie_Surface, NULL, screen->window, &saisie_Rect);
		SDL_BlitSurface(exit_Surface, NULL, screen->window, &exit_Rect);
		drawCursor(cursor, screen->window);
		SDL_Flip(screen->window);

		SDL_WaitEvent(&event);

            switch(event.type){
			case SDL_MOUSEMOTION:
				SDL_GetMouseState(&(cursor->location->x), &(cursor->location->y));
				break;
			case SDL_QUIT:
				exitCode=FINAL_MENU_QUIT;
				break;
			case SDL_KEYDOWN:
                if(event.key.keysym.sym==SDLK_RETURN || event.key.keysym.sym==SDLK_SPACE) //si on appui sur "entrée" alors on sort de la boucle
                        {
                            exitCode=FINAL_MENU_CONTINUE;
                        }
                        else if (event.key.keysym.sym<127 && event.key.keysym.sym>32)  //on inscrit le caractère avec le reste de la chaine uniquement si il se trouve dans la plage des caractères ASCII inscriptibles
                        {
                            chaine[i]=(char)event.key.keysym.unicode;
                            i++;
                        }
                        else if (event.key.keysym.sym==SDLK_BACKSPACE) //si l'on veut effacer le dernier caractère de la chaine
                        {
                            i--;
                            chaine[i]=0;
                        }
				break;
			default: break;
		}
            saisie_Surface=TTF_RenderText_Solid(mainFont, chaine, color);
            SDL_BlitSurface(saisie_Surface, NULL, screen->window, &saisie_Rect);
            SDL_Flip(screen->window);
            if (i>=longeur-1)
            {
                exitCode=FINAL_MENU_CONTINUE;
            }
	}

//Fin de tout les affichages
	SDL_FreeSurface(title_Surface);
	SDL_FreeSurface(msg1_Surface);
	SDL_FreeSurface(saisie_Surface);
	SDL_FreeSurface(exit_Surface);
	TTF_CloseFont(mainFont);
	TTF_CloseFont(titleFont);
	TTF_CloseFont(littleFont);
	return exitCode;
}
#endif
