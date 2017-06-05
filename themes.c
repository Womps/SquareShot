/**
* # themes.c
*/

#ifndef THEMES
#define THEMES
#include <stdbool.h>
#include <SDL/SDL.h>
#include "colors.c"
#include "images.c"


typedef struct{
	Color* backColor;	// Couleur du fond
	Color* targetColor; // Couleur de la cible
	Color* foreColor;
	char* fontPath;
	char useImage;
	char* imagePath;
	ImageLayout imageLayout;
}Theme;



//***************************************************************************
//                    		initTheme
//***************************************************************************
void initTheme(Theme* thm, Color* backColor, Color* targetColor, Color* foreColor, char* fontPath, bool useImg, char* imgPath, ImageLayout layout){
	thm->backColor=backColor;
	thm->targetColor=targetColor;
	thm->foreColor=foreColor;
	thm->fontPath=fontPath;
	thm->useImage=useImg;
	thm->imagePath=imgPath;
	thm->imageLayout=layout;
}


//****************************************************************************
//                    	setThemeBackground
//****************************************************************************
bool setThemeBackground(Theme* thm, SDL_Surface* back){
	SDL_FillRect(back, NULL, colorToUint(back->format, thm->backColor));
	if(thm->useImage){
		SDL_Surface* bmp = loadImage(thm->imagePath);
		if(bmp!=NULL) drawImage(bmp, back, thm->imageLayout);
		else return false;
		SDL_FreeSurface(bmp);
	}
	return true;
}


//****************************************************************************
//                    		setTheme
//****************************************************************************
void setTheme(Theme* thm, SDL_Surface* back, SDL_Surface* target){
	setThemeBackground(thm, back);
	SDL_FillRect(target, NULL, colorToUint(target->format, thm->targetColor));
}

#endif
