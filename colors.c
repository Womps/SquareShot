/**
* # colors.c
*/

#ifndef COLORS
#define COLORS
#include <SDL/SDL.h>

#define USE_DEFAULT_COLORMASKS 1
#if USE_DEFAULT_COLORMASKS//Si on utilise les masques de couleur par défaut de SDL (Pas besoins de se soucier du nombre de couleurs utilisées)
#define RMASK 0x00000000
#define GMASK 0x00000000
#define BMASK 0x00000000
#define AMASK 0x00000000
#else //Sinon si utilisations de masques 32bits RGBA
#if SDL_BYTEORDER == SDL_BIG_ENDIAN //Si architecture avec octet de poids fort en tete( Big Endian)
#define RMASK 0xff000000
#define GMASK 0x00ff0000
#define BMASK 0x0000ff00
#define AMASK 0x000000ff
#else //Sinon
#define RMASK 0x000000ff
#define GMASK 0x0000ff00
#define BMASK 0x00ff0000
#define AMASK 0xff000000
#endif
#endif


typedef struct{
	unsigned char R;		// Rouge
	unsigned char G;		// Vert
	unsigned char B;		// Bleu
}Color;

typedef struct{
	unsigned char R;		// Rouge
	unsigned char G;		// Vert
	unsigned char B;		// Bleu
	unsigned char A;		// Alpha = opacité
}ColorRGBA;



//***************************************************************************
//                    		initColor
//***************************************************************************
void initColor(Color* color, unsigned char red, unsigned char green, unsigned char blue){
	color->R=red;
	color->G=green;
	color->B=blue;
}

//***************************************************************************
//                    		initColorRGBA
//***************************************************************************
void initColorRGBA(ColorRGBA* color, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha){
	color->R=red;
	color->G=green;
	color->B=blue;
	color->A=alpha;
}

//***************************************************************************
//                    		makeColor
//***************************************************************************
Color* makeColor(unsigned char red, unsigned char green, unsigned char blue){
	Color* clr = (Color*)malloc(sizeof(Color));
	initColor(clr, red, green, blue);
	return clr;
}

//***************************************************************************
//                    		makeColorRGBA
//***************************************************************************
ColorRGBA* makeColorRGBA(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha){
	ColorRGBA* clr = (ColorRGBA*)malloc(sizeof(ColorRGBA));
	initColorRGBA(clr, red, green, blue, alpha);
	return clr;
}


//***************************************************************************
//                    		makeRGBAFromColor
//***************************************************************************
ColorRGBA* makeRGBAFromColor(Color* color){
	return makeColorRGBA(color->R, color->G, color->B, 255);
}

//***************************************************************************
//                    		uintToColor
//***************************************************************************
Color* uintToColor(unsigned int colorInt, SDL_PixelFormat* format){
	Color* clr=(Color*)malloc(sizeof(Color));
	SDL_GetRGB(colorInt, format, &(clr->R), &(clr->G), &(clr->B));
	return clr;
}

//***************************************************************************
//                    		uintToColorRGBA
//***************************************************************************
ColorRGBA* uintToColorRGBA(unsigned int colorInt, SDL_PixelFormat* format){
	ColorRGBA* clr=(ColorRGBA*)malloc(sizeof(ColorRGBA));
	SDL_GetRGBA(colorInt, format, &(clr->R), &(clr->G), &(clr->B), &(clr->A));
	return clr;
}

//***************************************************************************
//                    		colorToUint
//***************************************************************************
unsigned int colorToUint(SDL_PixelFormat* format, Color* clr){
	return SDL_MapRGB(format, clr->R, clr->G, clr->B);
}

//***************************************************************************
//                    		colorRGBAToUint
//***************************************************************************
unsigned int colorRGBAToUint(SDL_PixelFormat* format, ColorRGBA* clr){
	return SDL_MapRGBA(format, clr->R, clr->G, clr->B, clr->A);
}

//***************************************************************************
//                    		setColor
//***************************************************************************
void setColor(Color* color, SDL_Surface* surface){
	SDL_FillRect(surface, NULL, colorToUint(surface->format, color));
}

//***************************************************************************
//                    		setColorRGBA
//***************************************************************************
void setColorRGBA(ColorRGBA* color, SDL_Surface* surface){
	SDL_FillRect(surface, NULL, colorRGBAToUint(surface->format, color));
	SDL_SetAlpha(surface, SDL_SRCALPHA, color->A);
}

#endif


/*	Couleurs avec SDL :
	unsigned int SDL_MapRGB(const SDL_PixelFormat* format, char r, char g, char b);
	unsigned int SDL_MapRGBA(const SDL_PixelFormat* format, char r, char g, char b, char a);
	SDL_GetRGB(Uint32 pix, const SDL_PixelFormat* format, char* r, char* g, char* b, char* a);
	SDL_GetRGBA(Uint32 pix, const SDL_PixelFormat* format, char* r, char* g, char* b, char* a);
*/