/**
* # cursor.c
*/

#ifndef CURSOR
#define CURSOR

#include <SDL/SDL.h>
#include "display.c"
#include "colors.c"
#include "point.c"
#include "files.c"
#define CURSOR_DEFAULT_FILENAME "cursors/cursor.cur"
#define CURSOR_PLAYER1_FILENAME "cursors/cursor_player1.cur"
#define CURSOR_PLAYER2_FILENAME "cursors/cursor_player2.cur"

typedef struct{
	SDL_Surface* surface;
	SDL_Rect drawLocation;
	Point* location;
	Color* colorKey;
	char* imagePath;
	Point* dec;
}Cursor;


//***************************************************************************
//                    		initCursor
//***************************************************************************
void initCursor(Cursor* cursor, Color* colorKey, char* cursorImagePath){
	cursor->imagePath=cursorImagePath;
	cursor->colorKey=colorKey;
	cursor->location=NULL;
	cursor->surface=NULL;
	cursor->dec=NULL;
}


//*********************************************************************************
//							freeCursor
//	Procédure permettant de libérer l'esapace mémoire utilisée par un curseur
//*********************************************************************************
void freeCursor(Cursor* cursor){
	SDL_FreeSurface(cursor->surface);
	free(cursor->colorKey);
	free(cursor->location);
	free(cursor->dec);
}


//***************************************************************************
//                    		setCursor
//***************************************************************************
bool setCursor(Cursor* cursor, DisplayParameters* display){
	if(cursor->surface!=NULL) SDL_FreeSurface(cursor->surface);
	if(cursor->location!=NULL) free(cursor->location);
	if(cursor->dec!=NULL) free(cursor->dec);
	SDL_Surface* curImg=loadImage(cursor->imagePath);
	if (curImg==NULL) return false;
	cursor->surface = SDL_CreateRGBSurface(display->flags & (SDL_SWSURFACE | SDL_HWSURFACE), curImg->w, curImg->h, display->colorDepth, RMASK, GMASK, BMASK, AMASK); 
	if(cursor->surface==NULL) return false;
	drawImage(curImg, cursor->surface, IMAGE_LAYOUT_NONE);
	SDL_SetColorKey(cursor->surface, SDL_SRCCOLORKEY, colorToUint(cursor->surface->format, cursor->colorKey));
	cursor->dec=makePoint(cursor->surface->w/2,cursor->surface->h/2);
	cursor->location=makePoint(0,0);
	SDL_FreeSurface(curImg);
	return true;
}


//***************************************************************************
//                    		setCursor
//***************************************************************************
Cursor* makeCursor(char* cursorImagePath, Color* colorKey, DisplayParameters* display){
	Cursor* cur=(Cursor*)malloc(sizeof(Cursor));
	initCursor(cur, colorKey, cursorImagePath);
	setCursor(cur, display);
	return cur;
}


//***************************************************************************
//                    		setLocation
//***************************************************************************
void setCursorLoc(Cursor* cursor, int x, int y){
	cursor->location->x=x;
	cursor->location->y=y;
}


//*********************************************************************************
//                          drawCursor
//		 Procédure permettant de dessiner/placer un curseur sur surface
//*********************************************************************************
void drawCursor(Cursor* cursor, SDL_Surface* dest){
	cursor->drawLocation.x=(short)(cursor->location->x - cursor->dec->x);
	cursor->drawLocation.y=(short)(cursor->location->y - cursor->dec->y);
	SDL_BlitSurface(cursor->surface, NULL, dest, &(cursor->drawLocation));
}


#endif