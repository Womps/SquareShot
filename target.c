/**
* #target.c
*/

#ifndef TARGET
#define TARGET
#include <SDL/SDL.h>
#include "colors.c"
#include "display.c"
#define TARGET_DEFAULT_SIZE 64
#define TARGET_HARD_SIZE 48 // si la difficulté est Hard (3), la taille de la cible sera plus petite

typedef struct{
	SDL_Surface* surface;
	SDL_Rect location;
}Target;


//***************************************************************************
//                    		initTarget
//***************************************************************************
void initTarget(Target* target, DisplayParameters* display, int size){
	target->surface = SDL_CreateRGBSurface(display->flags & (SDL_SWSURFACE | SDL_HWSURFACE), size, size, display->colorDepth, RMASK, GMASK, BMASK, AMASK) ;    	// création de la surface correspondant à la cible
}
#endif
