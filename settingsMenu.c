/**
* # settingsMenu.c
*/

#ifndef SETTINGS_MENU
#define SETTINGS_MENU

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL\SDL.h>
#include <SDL\SDL_ttf.h>
#include <math.h>
#include "game.c"
#include "functions.c"
#include "display.c"
#include "cursor.c"
#include "target.c"
#include "themes.c"
#include "colors.c"
#include "images.c"
#include "files.c"
#include "point.c"

typedef enum {
	SETTINGS_MENU_NONE=0,
	SETTINGS_MENU_RELOAD=1,
	SETTINGS_MENU_EXIT=2,
	SETTINGS_MENU_QUIT=3
}SettingsMenuCode;


SettingsMenuCode showSettingsMenu(Screen* screen, DisplayParameters* display, Cursor* cursor, Cursor* p2cursor, Target* target, Theme* thm, GameParameters* game){

	SDL_Event event;
    //Déclarations pour le joystick
	if ( SDL_Init( SDL_INIT_JOYSTICK ) < 0 )
        return EXIT_FAILURE;
    SDL_Joystick *j1;
    j1 = SDL_JoystickOpen(0);
    SDL_JoystickEventState(SDL_ENABLE);

	bool canClick=true;
	SettingsMenuCode retCode=SETTINGS_MENU_NONE;
	setThemeBackground(thm, screen->background);
	SDL_PollEvent(&event);

	//pour la gestion de la souris
	if(game->commandes == 1) {
        SDL_GetMouseState(&(cursor->location->x), &(cursor->location->y));
	}
	// ou du joystick
	else {
        (cursor->location->x) = 320;
        (cursor->location->y) = 240;
	}

	int screenWidth=display->screenWidth;
	int screenHeight=display->screenHeight;
	int sdlModesPos=-1;
	int sdlModesCount=0;
	SDL_Rect** modes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);
	int i;
	for(i=0; modes[i]; ++i){
		sdlModesCount++;
		if ((modes[i]->w==screenWidth) && (modes[i]->h==screenHeight)) sdlModesPos=i;
	}
	char mode_str[]="0000x0000";
	if(sdlModesPos);
	bool rValChange;

	//Mode plein ecran
	bool fullScreen=false;
	if(display->flags & SDL_FULLSCREEN) fullScreen=true;
	char full_str[]="non";
	if(fullScreen) sprintf(full_str,"oui");

	//Difficulté
	char dVal_str[]="Moyenne";
    if(game->difficulty == 1) {
        sprintf(dVal_str,"Facile");
        game->coeff_diff = 0;
    }
    else if (game->difficulty == 2) game->coeff_diff = 2;
    else if (game->difficulty == 3) {
        sprintf(dVal_str,"Hard");
        game->coeff_diff = 2;
    }

	//Commandes
    bool cmd=false;
	if(game->commandes == 2) cmd=true;
	char cVal_str[]="Souris/Clavier";
	if(cmd) sprintf(cVal_str,"Manette");

	//Sensibilité du 'clavier-souris'
	if(game->player2KBSensitivity > game->player2KBMaxSensitivty) game->player2KBSensitivity = game->player2KBMaxSensitivty;
	int sensitivStep = game->player2KBMaxSensitivty / 100;
	int sVal_percent=100 - (game->player2KBSensitivity / sensitivStep);
	char sVal_str[]="000%";
	bool sValChange;


	TTF_Font* font = TTF_OpenFont(thm->fontPath, 24);
	TTF_Font* font2 = TTF_OpenFont(thm->fontPath, 16);
	SDL_Color color= {thm->foreColor->R, thm->foreColor->G, thm->foreColor->B};
	SDL_Color ops_color= {255, 32, 64};

	SDL_Surface* title_Surface=TTF_RenderText_Solid(font, "PARAMETRES", color);
	SDL_Surface* dTxt_Surface=TTF_RenderText_Solid(font2, "Difficulté : ", color);//création de la surface pour "difficulté"
	SDL_Surface* dSub_Surface=TTF_RenderText_Solid(font2, " - ", ops_color);// création de la surface pour diminuer la difficulté
	SDL_Surface* dVal_Surface=TTF_RenderText_Solid(font2, dVal_str, color);//création de la surface pour afficher la difficulté
	SDL_Surface* dAdd_Surface=TTF_RenderText_Solid(font2, " + ", ops_color);//création de la surface pour augmenter la difficulté
	SDL_Surface* sTxt_Surface=TTF_RenderText_Solid(font2, "Sensibilité joueur 2: ", color);
	SDL_Surface* sSub_Surface=TTF_RenderText_Solid(font2, "- ", ops_color);
	SDL_Surface* sVal_Surface=TTF_RenderText_Solid(font2, sVal_str, color);
	SDL_Surface* sAdd_Surface=TTF_RenderText_Solid(font2, " +", ops_color);
	SDL_Surface* rTxt_Surface=TTF_RenderText_Solid(font2, "Résolution: ", color);
	SDL_Surface* rVal_Surface=TTF_RenderText_Solid(font2, mode_str, color);
	SDL_Surface* rSet_Surface=TTF_RenderText_Solid(font2, " Appliquer", color);
	SDL_Surface* fTxt_Surface=TTF_RenderText_Solid(font2, " Plein écran:  ", color);
	SDL_Surface* fVal_Surface=TTF_RenderText_Solid(font2, full_str, ops_color);
	SDL_Surface* cTxt_Surface=TTF_RenderText_Solid(font2, "Commande: ", color);//création de la surface pour "commande"
	SDL_Surface* cVal_Surface=TTF_RenderText_Solid(font2, cVal_str, ops_color);//création de la surface pour afficher la commande utilisée
	SDL_Surface* back_Surface=TTF_RenderText_Solid(font, "Retour", color);

	int dSurfaceWidth = (dTxt_Surface->w + dSub_Surface->w + dVal_Surface->w + dAdd_Surface->w);//taille de la surface totale de difficulté
	int sSurfaceWidth = (sTxt_Surface->w + sSub_Surface->w + sVal_Surface->w + sAdd_Surface->w);
	int rSurfaceWidth = (rTxt_Surface->w + sSub_Surface->w + rVal_Surface->w + sAdd_Surface->w + rSet_Surface->w);
	int fSurfaceWidth = (fTxt_Surface->w + fVal_Surface->w);
	int cSurfaceWidth = (cTxt_Surface->w + cVal_Surface->w);//taille de la surface totale des commandes

	SDL_Rect title_Rect={(screen->window->w - title_Surface->w)/2, (screen->window->h /8) ,0 , 0};
	SDL_Rect dTxt_Rect={(screen->window->w - dSurfaceWidth)/2, (screen->window->h /8) * 2 , 0, 0};//création du rectangle des surfaces pour placement de "difficulté"
	SDL_Rect dSub_Rect={dTxt_Rect.x + dTxt_Surface->w, (screen->window->h /8) * 2 , 0, 0};//création du rectangle  des surfaces pour placement de l'augmentation de difficulté
    SDL_Rect dVal_Rect={dSub_Rect.x + dSub_Surface->w, (screen->window->h /8) * 2 , 0, 0};//création du rectangle des surfaces pour placement de la valeur de difficulté
    SDL_Rect dAdd_Rect={dVal_Rect.x + dVal_Surface->w, (screen->window->h /8) * 2 , 0, 0};//création du rectangle des surfaces pour placement de la diminution de difficulté
	SDL_Rect sTxt_Rect={(screen->window->w - sSurfaceWidth)/2, (screen->window->h /8) * 3 , 0, 0};
	SDL_Rect sSub_Rect={sTxt_Rect.x + sTxt_Surface->w, (screen->window->h /8) * 3 , 0, 0};
	SDL_Rect sVal_Rect={sSub_Rect.x + sSub_Surface->w, (screen->window->h /8) * 3 , 0, 0};
	SDL_Rect sAdd_Rect={sVal_Rect.x + sVal_Surface->w, (screen->window->h /8) * 3 , 0, 0};
	SDL_Rect rTxt_Rect={(screen->window->w - rSurfaceWidth)/2, (screen->window->h /8) * 4 , 0, 0};
	SDL_Rect rSub_Rect={rTxt_Rect.x + rTxt_Surface->w, (screen->window->h /8) * 4 , 0, 0};
	SDL_Rect rVal_Rect={rSub_Rect.x + sSub_Surface->w, (screen->window->h /8) * 4 , 0, 0};
	SDL_Rect rAdd_Rect={rVal_Rect.x + rVal_Surface->w, (screen->window->h /8) * 4 , 0, 0};
	SDL_Rect rSet_Rect={rAdd_Rect.x + sAdd_Surface->w, (screen->window->h /8) * 4 , 0, 0};
	SDL_Rect fTxt_Rect={(screen->window->w - fSurfaceWidth)/2, (screen->window->h /8) * 5 , 0, 0};
	SDL_Rect fVal_Rect={fTxt_Rect.x + fTxt_Surface->w, (screen->window->h /8) * 5 , 0, 0};
	SDL_Rect cTxt_Rect={(screen->window->w - cSurfaceWidth)/2, (screen->window->h /8) * 6 , 0, 0};//création du rectangle pour placement de "commande"
	SDL_Rect cVal_Rect={cTxt_Rect.x + cTxt_Surface->w, (screen->window->h /8) * 6 , 0, 0};//création du rectangle pour placement de la commande utilisée
	SDL_Rect back_Rect={(screen->window->w - back_Surface->w)/2, (screen->window->h /8) * 7, 0, 0};

	sValChange=true; //On affiche la valeur réelle de la sensibilité (définie à "000%" pour les calculs)
	rValChange=true;

	int sLeft=sSub_Rect.x + sSub_Surface->w;
	int sRight=sAdd_Rect.x;
	int rLeft=rSub_Rect.x + sSub_Surface->w;
	int rRight=rAdd_Rect.x;

	while(!retCode){

		if(sValChange){
			sValChange=false;
			game->player2KBSensitivity=(sensitivStep * ( 100 - sVal_percent));
			SDL_FreeSurface(sVal_Surface);
			sprintf(sVal_str,"%d%%", sVal_percent);
			sVal_Surface=TTF_RenderText_Solid(font2, sVal_str, color);
			sVal_Rect.x=(sLeft + (((sRight - sLeft) - sVal_Surface->w) / 2));
		}

		if(rValChange){
			rValChange=false;
			SDL_FreeSurface(rVal_Surface);
			if(sdlModesPos==-1){
				screenWidth=display->screenWidth;
				screenHeight=display->screenHeight;
			}
			screenWidth=modes[sdlModesPos]->w;
			screenHeight=modes[sdlModesPos]->h;
			sprintf(mode_str,"%dx%d", screenWidth, screenHeight);
			rVal_Surface=TTF_RenderText_Solid(font2, mode_str, color);
			rVal_Rect.x=(rLeft + (((rRight - rLeft) - rVal_Surface->w) / 2));
		}

        //application de la taille du curseur selon la difficulté
        if (game->difficulty>=3)    {initTarget(target, display, TARGET_HARD_SIZE);}
        else     {initTarget(target, display, TARGET_DEFAULT_SIZE);}
        setTheme(thm, screen->background, target->surface);

		//fond
		SDL_BlitSurface(screen->background, NULL, screen->window, NULL);
		//Titre
		SDL_BlitSurface(title_Surface, NULL, screen->window, &title_Rect);
		//Difficulté - Affichage
		SDL_BlitSurface(dTxt_Surface, NULL, screen->window, &dTxt_Rect);
		SDL_BlitSurface(dSub_Surface, NULL, screen->window, &dSub_Rect);
		SDL_BlitSurface(dVal_Surface, NULL, screen->window, &dVal_Rect);
		SDL_BlitSurface(dAdd_Surface, NULL, screen->window, &dAdd_Rect);
		//Sensibilité
		SDL_BlitSurface(sTxt_Surface, NULL, screen->window, &sTxt_Rect);
		SDL_BlitSurface(sSub_Surface, NULL, screen->window, &sSub_Rect);
		SDL_BlitSurface(sVal_Surface, NULL, screen->window, &sVal_Rect);
		SDL_BlitSurface(sAdd_Surface, NULL, screen->window, &sAdd_Rect);
		//Résolution
		SDL_BlitSurface(rTxt_Surface, NULL, screen->window, &rTxt_Rect);
		SDL_BlitSurface(sSub_Surface, NULL, screen->window, &rSub_Rect);
		SDL_BlitSurface(rVal_Surface, NULL, screen->window, &rVal_Rect);
		SDL_BlitSurface(sAdd_Surface, NULL, screen->window, &rAdd_Rect);
		SDL_BlitSurface(rSet_Surface, NULL, screen->window, &rSet_Rect);
		//Plein ecran
		SDL_BlitSurface(fTxt_Surface, NULL, screen->window, &fTxt_Rect);
		SDL_BlitSurface(fVal_Surface, NULL, screen->window, &fVal_Rect);
		//Commandes - Affichage
		SDL_BlitSurface(cTxt_Surface, NULL, screen->window, &cTxt_Rect);
		SDL_BlitSurface(cVal_Surface, NULL, screen->window, &cVal_Rect);
		//Retour
		SDL_BlitSurface(back_Surface, NULL, screen->window, &back_Rect);
		//Curseur
		drawCursor(cursor, screen->window);
		SDL_Flip(screen->window);

		SDL_WaitEvent(&event);
		// Si la commande via souris est sélectionnée
		if(game->commandes == 1) {
            switch(event.type){
                case SDL_MOUSEMOTION:
                    SDL_GetMouseState(&(cursor->location->x), &(cursor->location->y));
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if(canClick){
                        canClick=false;
                        SDL_GetMouseState(&(cursor->location->x), &(cursor->location->y));
                        if(containsP(&dSub_Rect, cursor->location)){
                            if(game->difficulty != 1) { // si la difficulté n'est pas 1, on peut la diminuer
                                game->difficulty--; //diminution de la difficulté
                            }
                            retCode=SETTINGS_MENU_RELOAD;
                        }
                        else if(containsP(&dAdd_Rect, cursor->location)) {
                            if(game->difficulty != 3) {//si la difficulté n'est pas 3, on peut l'augmenter
                                game->difficulty++;//augmentation de la difficulté
                            }
                            retCode=SETTINGS_MENU_RELOAD;
                        }
                        else if(containsP(&sSub_Rect, cursor->location)){
                            if(sVal_percent>0){
                                sVal_percent--;
                                sValChange=true;
                            }
                        }else if(containsP(&sAdd_Rect, cursor->location)){
                            if(sVal_percent<100){
                                sVal_percent++;
                                sValChange=true;
                            }
                        }else if(containsP(&rSub_Rect, cursor->location)){
                            sdlModesPos++;  // inversé car SDL_ListModes() donne généralement la liste des résolutions de la plus grande à la plus petite
                            if(sdlModesPos>=sdlModesCount) sdlModesPos=0;
                            rValChange=true;
                        }
                        else if(containsP(&rAdd_Rect, cursor->location)){
                            sdlModesPos--;
                            if(sdlModesPos<0) sdlModesPos=sdlModesCount-1;
                            rValChange=true;
                        }
                        else if(containsP(&rSet_Rect, cursor->location)){
                            switchVideoMode(screenWidth, screenHeight, fullScreen, display, screen, thm);
                            SDL_GetMouseState(&(cursor->location->x), &(cursor->location->y));
                            if(p2cursor!=NULL){ // évite la disparition du 2ème curseur s'il y a une partie 2 joueurs en cours
                                p2cursor->location->x=(screen->window->w / 2);
                                p2cursor->location->y=(screen->window->h / 2);
                            }
                            if(target!=NULL){ // évite la disparition du 2ème curseur s'il y a une partie 2 joueurs en cours
                                target->location.x=rand()%(screen->window->w - target->surface->w);
                                target->location.y=rand()%(screen->window->h - target->surface->h);
                            }
                            retCode=SETTINGS_MENU_RELOAD;
                        }
                        else if(containsP(&fVal_Rect, cursor->location)){
                            if(fullScreen) fullScreen=false;
                            else fullScreen=true;
                            switchVideoMode(display->screenWidth, display->screenHeight, fullScreen, display, screen, thm);
                            retCode=SETTINGS_MENU_RELOAD;
                        }
                        else if(containsP(&cVal_Rect, cursor->location)) {
                            if(game->commandes==2) game->commandes=1; //si la commande joystick est sélectionnée, passage à la souris
                            else game->commandes=2; // ou inversement
                            retCode=SETTINGS_MENU_RELOAD; //rechargement du menu settings
                        }
                        else if(containsP(&back_Rect, cursor->location)){
                            retCode=SETTINGS_MENU_EXIT;
                        }
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    canClick=true;
                    break;
                case SDL_QUIT:
                    retCode=SETTINGS_MENU_QUIT;
                    break;
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym==SDLK_ESCAPE) retCode=SETTINGS_MENU_EXIT;
                    break;
                default: break;
            }
        }
        // Si le mode joystick est sélectionné
        else {
            switch(event.type){
                case SDL_JOYAXISMOTION:
                    if((SDL_JoystickGetAxis(j1,0) / 100 + 320) < 0) (cursor->location->x) = 0; // si le curseur sort de l'écran à gauche, on le place au bord gauche
                    else (cursor->location->x) = SDL_JoystickGetAxis(j1,0) / 100 + 320;// sinon on l'affiche à sa position horizontale
                    if((SDL_JoystickGetAxis(j1,1) / 100 + 240) < 0) (cursor->location->y) = 0;// si le curseur sort de l'écran en haut, on le place au bord en haut
                    else (cursor->location->y) = SDL_JoystickGetAxis(j1,1) / 100 + 240;// sinon on l'affiche à sa position verticale
                    if((SDL_JoystickGetAxis(j1,0) / 100 + 320) > 640) (cursor->location->x) = 640;// si le curseur sort de l'écran en bas, on le place au bord en bas
                    if((SDL_JoystickGetAxis(j1,1) / 100 + 240) > 480) (cursor->location->y) = 480;// si le curseur sort de l'écran à droite, on le place au bord droit
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

                        if(containsP(&dSub_Rect, cursor->location)){
                            if(game->difficulty != 1) {
                                game->difficulty--;
                            }
                            retCode=SETTINGS_MENU_RELOAD;
                        }
                        else if(containsP(&dAdd_Rect, cursor->location)) {
                            if(game->difficulty != 3) {
                                game->difficulty++;
                            }
                            retCode=SETTINGS_MENU_RELOAD;
                        }
                        else if(containsP(&sSub_Rect, cursor->location)){
                            if(sVal_percent>0){
                                sVal_percent--;
                                sValChange=true;
                            }
                        }else if(containsP(&sAdd_Rect, cursor->location)){
                            if(sVal_percent<100){
                                sVal_percent++;
                                sValChange=true;
                            }
                        }else if(containsP(&rSub_Rect, cursor->location)){
                            sdlModesPos++;  // inversé car SDL_ListModes() donne généralement la liste des résolutions de la plus grande à la plus petite
                            if(sdlModesPos>=sdlModesCount) sdlModesPos=0;
                            rValChange=true;
                        }
                        else if(containsP(&rAdd_Rect, cursor->location)){
                            sdlModesPos--;
                            if(sdlModesPos<0) sdlModesPos=sdlModesCount-1;
                            rValChange=true;
                        }
                        else if(containsP(&rSet_Rect, cursor->location)){
                            switchVideoMode(screenWidth, screenHeight, fullScreen, display, screen, thm);
                            (cursor->location->x) = SDL_JoystickGetAxis(j1, 0) / 100 + 320;
                            (cursor->location->y) = SDL_JoystickGetAxis(j1, 1) / 100 + 320;
                            if(p2cursor!=NULL){ // évite la disparition du 2ème curseur s'il y a une partie 2 joueurs en cours
                                p2cursor->location->x=(screen->window->w / 2);
                                p2cursor->location->y=(screen->window->h / 2);
                            }
                            if(target!=NULL){ // évite la disparition du 2ème curseur s'il y a une partie 2 joueurs en cours
                                target->location.x=rand()%(screen->window->w - target->surface->w);
                                target->location.y=rand()%(screen->window->h - target->surface->h);
                            }
                            retCode=SETTINGS_MENU_RELOAD;
                        }
                        else if(containsP(&fVal_Rect, cursor->location)){
                            if(fullScreen) fullScreen=false;
                            else fullScreen=true;
                            switchVideoMode(display->screenWidth, display->screenHeight, fullScreen, display, screen, thm);
                            retCode=SETTINGS_MENU_RELOAD;
                        }
                        else if(containsP(&cVal_Rect, cursor->location)) {
                            if(game->commandes==2) game->commandes=1;
                            else game->commandes=2;
                            retCode=SETTINGS_MENU_RELOAD;
                        }
                        else if(containsP(&back_Rect, cursor->location)){
                            retCode=SETTINGS_MENU_EXIT;
                        }
                    }
                    break;
                case SDL_JOYBUTTONUP:
                    canClick=true;
                    break;
                case SDL_QUIT:
                    retCode=SETTINGS_MENU_QUIT;
                    break;
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym==SDLK_ESCAPE) retCode=SETTINGS_MENU_EXIT;
                    break;
                default: break;
            }
        }
	}

        SDL_FreeSurface(title_Surface);
        SDL_FreeSurface(dTxt_Surface);
        SDL_FreeSurface(dSub_Surface);
        SDL_FreeSurface(dVal_Surface);
        SDL_FreeSurface(dAdd_Surface);
        SDL_FreeSurface(sTxt_Surface);
        SDL_FreeSurface(sSub_Surface);
        SDL_FreeSurface(sVal_Surface);
        SDL_FreeSurface(sAdd_Surface);
        SDL_FreeSurface(back_Surface);
        SDL_FreeSurface(rTxt_Surface);
        SDL_FreeSurface(rVal_Surface);
        SDL_FreeSurface(rSet_Surface);
        SDL_FreeSurface(fTxt_Surface);
        SDL_FreeSurface(fVal_Surface);
        SDL_FreeSurface(cTxt_Surface);//fermeture de la surface de commande
        SDL_FreeSurface(cVal_Surface);//fermeture de la surface de l'affichage de commande

        if(game->commandes == 2) SDL_JoystickClose(j1);// si le joystick est ouvert (sélectionné), on le ferme

        TTF_CloseFont(font);
        TTF_CloseFont(font2);
        return retCode;
}

#endif
