/**
* # point.c
*/

#include <stdlib.h>

#ifndef POINTS
#define POINTS

typedef struct{
	int x;
	int y;
}Point;


//***************************************************************************
//                    		initPoint
//***************************************************************************
void initPoint(Point* point, int x, int y){
	point->x=x;
	point->y=y;
}


//***************************************************************************
//                    		makePoint
//***************************************************************************
Point* makePoint(int x, int y){
	Point* point=(Point*)malloc(sizeof(Point));
	point->x=x;
	point->y=y;
	return point;
}

#endif
