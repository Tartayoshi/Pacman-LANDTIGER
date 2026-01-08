#include "LPC17xx.h"
#ifndef BITMAP_H
#define BITMAP_H


enum rotation{NOROT, ROT90, MIRROR, MIRRORROT90, MIRRORY, MIRRORYROT90};
extern void BITMAP_draw(const uint16_t buffer[], uint16_t posX, uint16_t posY, uint16_t sizeX, uint16_t sizeY, enum rotation rotation);
#endif
