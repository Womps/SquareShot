/** Vérifier si erreurs de codage : cd \Users\HG\Projet\ && gcc -c functions.c -o functions.o -lmingw32 -lSDLmain -lSDL -Wall -Werror
* # functions.c
*/

#ifndef FUNCTIONS
#define FUNCTIONS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "display.c"
#include "target.c"
#include "themes.c"
#include "colors.c"
#include "cursor.c"
#include "point.c"


#define EXPLODE_ANIM_LENGTH 75 // valeur arbitraire pour la "durée et l'expansion" de l'animation
#define EXPLODE_LINE_HEIGT 8

//*********************************************************************************
//                            	contains
//	Fonction permettant de savoir si un rectangle contient le point (x,y)
//*********************************************************************************
bool contains(SDL_Rect* rect, int x, int y){
	if ((x >= rect->x) && (y >= rect->y) && (x < (rect->x + rect->w)) && (y < (rect->y + rect->h))) return true;
	else return false;
}


//*********************************************************************************
//                            	containsP
//		 Fonction permettant de savoir si un rectangle contient un point
//*********************************************************************************
bool containsP(SDL_Rect* rect, Point* pt){
	if ((pt->x >= rect->x) && (pt->y >= rect->y) && (pt->x < (rect->x + rect->w)) && (pt->y < (rect->y + rect->h))) return true;
	else return false;
}


//*********************************************************************************
//                            	explode
//		Procédure permettant de simuler une explosion de la cible
//*********************************************************************************
void explode(Target* target, Screen* screen, GameParameters* game, Cursor* cursor1, Cursor* cursor2, DisplayParameters* display, Theme* thm, int blocSize, int syncTime){

	ColorRGBA* blocsColor = makeRGBAFromColor(thm->targetColor);
	int dec = (unsigned char)((510) /(EXPLODE_ANIM_LENGTH)) + 1;
	int linesNb = (int)(target->surface->w)/EXPLODE_LINE_HEIGT;
	int blocsNb = linesNb * linesNb;
	SDL_Surface* blocs[blocsNb];
	unsigned int lastTime;
	SDL_Rect b;
	int i;
	int x;
	int y;

    SDL_Joystick *j1;
    SDL_Joystick *j2;
    j1 = SDL_JoystickOpen(0);
    j2 = SDL_JoystickOpen(1);
    SDL_JoystickEventState(SDL_ENABLE);
//Ceci est utilisé pour l'affichage lorsque le carré explose

	char level_Str[15];
	char point1_Str [20];//Initialisation des chaines de caractere de l'affichage
	char point2_Str [20];
	char score1_Str[40];
	char score2_Str[40];
	SDL_Color color={thm->foreColor->R, thm->foreColor->G, thm->foreColor->B};
	TTF_Font* mainFont = TTF_OpenFont(thm->fontPath, 20);
	sprintf(level_Str, "Niveau : %d", game->level);
	if(game->twoPlayers)//S'il y a 2 joueurs, on fait tout afficher
	{
		sprintf(score1_Str, "Joueur 1 : %d / %d", game->pointsNumber ,game->pointsLimit);
		sprintf(score2_Str, "Joueur 2 : %d / %d", game->player2PointsNumber ,game->pointsLimit);
		sprintf(point1_Str, "Score J1 : %d", game->score); //Ajout des scores pour le joueur 1 et 2
		sprintf(point2_Str, "Score J2 : %d", game->player2Score);

	}
	else//Sinon on fait seulement afficher les points et le score pour un joueur
        {
            sprintf(score1_Str, "Nombre de carres : %d / %d", game->pointsNumber ,game->pointsLimit);
            sprintf(point1_Str, "Score : %d", game->score);
        }

	SDL_Surface* level_Surface=TTF_RenderText_Solid(mainFont, level_Str, color);
	SDL_Surface* score1_Surface=TTF_RenderText_Solid(mainFont, score1_Str, color);
	SDL_Surface* point1_Surface=TTF_RenderText_Solid(mainFont, point1_Str, color);
	SDL_Surface* score2_Surface=TTF_RenderText_Solid(mainFont, score2_Str, color);
	SDL_Surface* point2_Surface=TTF_RenderText_Solid(mainFont, point2_Str, color);

//On affiche tout sur le bord du cadre et décalé vers le bas de la taille du rectangle d'affichage du dessus

	SDL_Rect level_Rect={10, 10 ,0 , 0};
	SDL_Rect score1_Rect={10, level_Rect.y + level_Surface->h + 10, 0, 0};
    SDL_Rect point1_Rect={10, score1_Rect.y + score1_Surface->h + 10, 0, 0};
    SDL_Rect score2_Rect={10, point1_Rect.y + point1_Surface->h + 10, 0, 0};
    SDL_Rect point2_Rect={10, score2_Rect.y + score2_Surface->h + 10, 0, 0};



	for(i=0; i<blocsNb; i++){
		blocs[i] =  SDL_CreateRGBSurface((SDL_HWSURFACE | SDL_SWSURFACE) & display->flags, (blocSize/2)*((i%2)+1),(blocSize/2)*((i%2)+1), display->colorDepth, RMASK, GMASK, BMASK, AMASK);
		setColorRGBA(blocsColor, blocs[i]);
	}
	for(i=0; i<EXPLODE_ANIM_LENGTH; i++){
		SDL_BlitSurface(screen->background, NULL, screen->window, NULL);
		if((blocsColor->A >= dec) && (i >(EXPLODE_ANIM_LENGTH/2)) ){ // A partir de la moitié de l'animation
			(blocsColor->A)-= dec; // on décrémente l'opacité pour qu'au fil de l'explosion les blocs "s'éffacent"
		}
//Pour l'affichage 1 joueur
		lastTime=SDL_GetTicks() + syncTime;
		SDL_BlitSurface(level_Surface, NULL, screen->window, &level_Rect);
		SDL_BlitSurface(score1_Surface, NULL, screen->window, &score1_Rect);
		SDL_BlitSurface(point1_Surface, NULL, screen->window, &point1_Rect);


//et s'il y a 2 joueurs, on ajoute
		if(game->twoPlayers)
        {
               SDL_BlitSurface(score2_Surface, NULL, screen->window, &score2_Rect);
               SDL_BlitSurface(point2_Surface, NULL, screen->window, &point2_Rect);
        }

		for(y=1; y<=linesNb; y++){
			for(x=1; x<=linesNb; x++){
				b.x = target->location.x - (i*(EXPLODE_LINE_HEIGT-2)/2) + ((EXPLODE_LINE_HEIGT*(x-1)) * (i/(EXPLODE_LINE_HEIGT-2))) ;
				b.y = target->location.y - (i*(EXPLODE_LINE_HEIGT-2)/2) + ((EXPLODE_LINE_HEIGT*(y-1)) * (i/(EXPLODE_LINE_HEIGT-2)));
				SDL_BlitSurface(blocs[(x*y)-1], NULL, screen->window, &b);
				setColorRGBA(blocsColor, blocs[(x*y)-1]); // on change la couleur du bloc (avec nouvelle opacité) pour sa prochaine position
			}
		}
		//gestion mouvements souris:
		SDL_PollEvent(NULL);
		if(game->commandes == 1)
            SDL_GetMouseState(&(cursor1->location->x), &(cursor1->location->y));
        else {
            if((SDL_JoystickGetAxis(j1,0) / 100 + 320) < 0) (cursor1->location->x) = 0;
            else (cursor1->location->x) = SDL_JoystickGetAxis(j1,0) / 100 + 320;
            if((SDL_JoystickGetAxis(j1,1) / 100 + 240) < 0) (cursor1->location->y) = 0;
            else (cursor1->location->y) = SDL_JoystickGetAxis(j1,1) / 100 + 240;
            if((SDL_JoystickGetAxis(j1,0) / 100 + 320) > 640) (cursor1->location->x) = 640;
            if((SDL_JoystickGetAxis(j1,1) / 100 + 240) > 480) (cursor1->location->y) = 480;
        }
		if(game->twoPlayers) {
                if(game->commandes == 1)
                    SDL_GetMouseState(&(cursor1->location->x), &(cursor1->location->y));
                else {
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
                }
                drawCursor(cursor2,screen->window);
		}
		drawCursor(cursor1,screen->window);

   		SDL_Flip(screen->window);
		while(SDL_GetTicks() < lastTime){};
	}
	for(i=0; i<blocsNb; i++){
		SDL_FreeSurface(blocs[i]);
	}

//Libération de toute les surfaces
	SDL_FreeSurface(level_Surface);
	SDL_FreeSurface(score1_Surface);
	SDL_FreeSurface(point1_Surface);

	SDL_FreeSurface(score2_Surface);
	SDL_FreeSurface(point2_Surface);

	TTF_CloseFont(mainFont);
}



//*********************************************************************************
//                            	printModes
//		Procédure permettant "d'afficher" (sur le flux défini en sortie) les
//			résolutions disponibles sur la configuration actuelle.
//*********************************************************************************
void printModes(){
	int i;
	SDL_Rect** modes;
	printf("Modes disponibles :\n");
	modes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);
	for(i=0; modes[i]; ++i) printf("  %d x %d\n", modes[i]->w, modes[i]->h);
}



//*********************************************************************************
//                            	reopen
//	Procédure permettant de rediriger le flux standard en sortie (stdout+stderr)
//	ou en entrée (stdin) vers le chemin 'path' (fichier ou nom d'un flux, "CON"
//					pour la console sous Windows).
//*********************************************************************************
void reopen(char* path, bool out){
	if(out){
		freopen(path, "w", stdout);
		freopen(path, "w", stderr);
	}else freopen(path, "r", stdin);
}



#endif
