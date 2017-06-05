/**
* # display.c
*/

#ifndef DISPLAY
#define DISPLAY
#include <stdbool.h>
#include <SDL/SDL.h>
#include "themes.c"

#define DISPLAY_DEFAULT_WIDTH 640
#define DISPLAY_DEFAULT_HEIGHT 480
#define DISPLAY_DEFAULT_COLORDEPTH 32
#define DISPLAY_DEFAULT_FPS 100

typedef struct DisplayParameters{
	int screenWidth;	// largeur de la fenêtre
	int screenHeight;	// hauteur de la fenetre
	int colorDepth;		// nombre de bits par pixel
	int fps;    		// nombre d'image par seconde
	int syncTime;    	// temps d'attente avant rafraichissement = 1000/fps
	unsigned int flags;	// flags pour la fenetre (ex: doublebuffer, resizable,...)
}DisplayParameters;


typedef struct Screen{
	SDL_Surface* window;
	SDL_Surface* background;
}Screen;


//***************************************************************************
//                    	initDisplayParameters
// Procédure permettant d'initialiser les valeurs d'une structure de type
//                    	'DisplayParameters'
//***************************************************************************
void initDisplayParameters(DisplayParameters* params, int width, int height, int clrDepth, int fps, unsigned int flags){
	params->screenWidth=width;
	params->screenHeight=height;
	params->colorDepth=clrDepth;
	if(fps<1) fps=1;
	params->fps=fps;
	params->syncTime=(int)(1000/fps);
	params->flags=flags;
}


//*********************************************************************************
//                    	setDisplayParameters
// Procédure permettant d'appliquer les valeurs d'une structure de type
//            	'DisplayParameters' à une surface 'SDL_Surface'
//*********************************************************************************
bool setDisplayParameters(DisplayParameters* display, Screen* screen){
	if(screen->window!=NULL) SDL_FreeSurface(screen->window);
	if(screen->background!=NULL) SDL_FreeSurface(screen->background);
	screen->window=SDL_SetVideoMode(display->screenWidth, display->screenHeight, display->colorDepth, display->flags);
	screen->background=SDL_CreateRGBSurface(display->flags & (SDL_SWSURFACE | SDL_HWSURFACE), display->screenWidth,
										    display->screenHeight, display->colorDepth, RMASK, GMASK, BMASK, AMASK);
	if(screen->window==NULL) return false;
	else return true ;
}



//***************************************************************************
//                    		initScreen
//	  Procédure permettant d'initialiser une structure de type 'Screen'
//***************************************************************************
void initScreen(Screen* screen){
	screen->window=NULL;
	screen->background=NULL;
}



//*********************************************************************************
//                    		switchVideoMode
//*********************************************************************************
void switchVideoMode(int width, int height, bool fullScreen, DisplayParameters* display, Screen* screen, Theme* thm){
	initDisplayParameters(display, width, height, display->colorDepth, display->fps,
						 (display->flags & ~SDL_FULLSCREEN) | (SDL_FULLSCREEN * (unsigned int)fullScreen));
	setDisplayParameters(display, screen);
	setThemeBackground(thm, screen->background);
	SDL_BlitSurface(screen->background, NULL, screen->window, NULL);
}




#endif