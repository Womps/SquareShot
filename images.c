/**
* # images.c
*/

#ifndef IMAGES
#define IMAGES
#include <SDL/SDL.h>
#include "files.c"


typedef enum{
	IMAGE_LAYOUT_NOIMAGE=-1,
	IMAGE_LAYOUT_NONE=0,
	IMAGE_LAYOUT_CENTER=1,
	IMAGE_LAYOUT_TILE=2
} ImageLayout;  


//*********************************************************************************
//                            	loadImage
//		Fonction permettant de charger une image, retourne NULL en cas d'echec
//*********************************************************************************
SDL_Surface* loadImage(char* imagePath){
	if(imagePath!=NULL){
		if(fileExists(imagePath)){
			SDL_Surface* bmp = SDL_LoadBMP(imagePath);
			if(bmp!=NULL){
				SDL_Surface* final = SDL_DisplayFormat(bmp);
				SDL_FreeSurface(bmp);
				return final;
			}
		}
	}
	return NULL;
}


//*********************************************************************************
//                            	drawImage
//		 Procédure permettant de dessiner/placer une image dans une surface
//*********************************************************************************
void drawImage(SDL_Surface* image, SDL_Surface* dest, ImageLayout layout){
	SDL_Rect srcRect;
	SDL_Rect dstRect;

	srcRect.x=0;
	srcRect.y=0;
	srcRect.w=image->w;
	srcRect.h=image->h;

	if(layout==IMAGE_LAYOUT_TILE){
		int nbImgWidth = ((dest->w -1)/image->w)+1;
		int nbImgHeight = ((dest->h -1)/image->h)+1;
		int x, y;
		for(x=0; x<nbImgWidth; x++){
			dstRect.x = x*(image->w);
			for(y=0; y<nbImgHeight; y++){
				dstRect.y = y*(image->h);
				if((dstRect.x + image->w) > dest->w) srcRect.w=(dest->w - dstRect.x); else srcRect.w=image->w;
				if((dstRect.y + image->h) > dest->h) srcRect.h=(dest->h - dstRect.y); else srcRect.h=image->h;
				SDL_BlitSurface(image, &srcRect, dest, &dstRect);
			}
		}
	}
	else if(layout==IMAGE_LAYOUT_CENTER){
		dstRect.x=(dest->w - image->w) / 2;
		dstRect.y=(dest->h - image->h) / 2;
		if(image->w > dest->w) srcRect.w=dest->w - dstRect.x;
		if(image->h > dest->h) srcRect.h=dest->h - dstRect.y;
		SDL_BlitSurface(image, &srcRect, dest, &dstRect);
	}
	else if(layout==IMAGE_LAYOUT_NONE){
		dstRect.x=0;
		dstRect.y=0;
		if(image->w > dest->w) srcRect.w=dest->w;
		if(image->h > dest->h) srcRect.h=dest->h;
		SDL_BlitSurface(image, &srcRect, dest, &dstRect);
	}
}
#endif