#include "LPC17xx.h"
#ifndef MAPDATA_H
#define MAPDATA_H
enum PacTails{EMPT, WALL, PILL, SUPE, SPAW, PACM, BLIN}; //NORMALIZE WORDS TO W CHAR TO BETTER READBILITY OF MATRIX
extern enum PacTails level0[];

extern const unsigned int mapDimX;//tails
extern const unsigned int mapDimY; //tails

//extern const unsigned int tailSize; //px 8*8
extern const unsigned int pillSize; //px 2*2
extern unsigned int offset; //pill offset

extern const unsigned int startXmap; //px
extern const unsigned int startYmap;
#endif