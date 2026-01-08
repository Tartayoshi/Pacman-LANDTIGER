#include "bitmap.h"
#include "GLCD/GLCD.h" 

#include "Pacman/mapData.h" //for pacman effect
#include "Pacman/bitmapData.h" //for pacman effect


//Magenta == alpha
#define CHROMAKEY Magenta
#define USECHORMAKEY 1 //put it to 0 if you don't want chroma keying

void BITMAP_draw(const uint16_t buffer[], uint16_t posX, uint16_t posY, uint16_t sizeX, uint16_t sizeY, enum rotation rotation){
	int i, j;
	
	uint16_t startX = posX - sizeX / 2; //starting position for x
	uint16_t startY = posY - sizeY / 2; //starting position for y
	
	uint16_t x; // cursor x 
	uint16_t y; //cursot y
	
	switch(rotation){
		case NOROT: //pacman move right
			
			y = startY; //	cursor y
		
			for (i = 0; i < sizeY; i++){
				x = startX;
				
				//check if first pixel is outside the map, remove some glitches in pacman effect
				if( x < startXmap){
						x = mapDimX * tailSize + startXmap/* - sizeX / 2 - 1*/;
				}
				
				
				for(j = 0; j < sizeX; j++){
					if(USECHORMAKEY == 1 && buffer[i*sizeX + j] != CHROMAKEY){ //chroma key effect
						LCD_SetPoint(x, y, buffer[i*sizeX + j]);
					}
					x++;
					if(x > mapDimX * tailSize + startXmap){ //pacman effect
						x = startXmap;
					}else if( x < startXmap){
						x = mapDimX * tailSize + startXmap;
					}
				}
				y++;
			}
			
			break;
		
		case ROT90:
			y = startY; //	cursor y
		
			for (i = 0; i < sizeX; i++){
				x = startX;
				for(j = sizeY - 1; j >= 0; j--){
					if(USECHORMAKEY == 1 && buffer[j*sizeY + i] != CHROMAKEY){//chroma key effect
						LCD_SetPoint(x, y, buffer[j*sizeY + i]);
					}
					x++;
				}
				y++;
			}
			
			break;
		
		case MIRROR: //pacman move left
			y = startY; //	cursor y
		
			for (i = 0; i < sizeY; i++){
				x = startX;
				for(j = sizeX - 1; j >= 0; j--){
					if(USECHORMAKEY == 1 && buffer[i*sizeX + j] != CHROMAKEY){//chroma key effect
						LCD_SetPoint(x, y, buffer[i*sizeX + j]);
					}
					x++;
					
					if(x > mapDimX * tailSize + startXmap){ //pacman effect
						x = startXmap;
					}else if( x < startXmap){
					x = mapDimX * tailSize + startXmap;
					}
					
				}
				y++;
			}
			break;
		
		case MIRRORROT90:
			y = startY; //	cursor y
		
			for (i = sizeX - 1; i >= 0; i--){
				x = startX;
				for(j = 0; j < sizeY; j++){
					if(USECHORMAKEY == 1 && buffer[j*sizeY + i] != CHROMAKEY){//chroma key effect
						LCD_SetPoint(x, y, buffer[j*sizeY + i]);
					}
					x++;
				}
				y++;
			}
			break;
			
		case MIRRORY:
			y = startY; //	cursor y
		
			for (i = sizeY - 1; i >= 0; i--){
				x = startX;
				for(j = 0; j < sizeX; j++){
					if(USECHORMAKEY == 1 && buffer[i*sizeX + j] != CHROMAKEY){//chroma key effect
						LCD_SetPoint(x, y, buffer[i*sizeX + j]);
					}
					x++;
				
				}
				y++;
			}
			break;
		case MIRRORYROT90:
			y = startY; //	cursor y
		
			for (i = sizeY - 1; i >= 0; i--){
				x = startX;
				for(j = sizeX - 1; j >= 0; j--){
					if(USECHORMAKEY == 1 && buffer[i*sizeX + j] != CHROMAKEY){//chroma key effect
						LCD_SetPoint(x, y, buffer[i*sizeX + j]);
					}
					x++;
				}
				y++;
			}
			break;
		
	}
	
}

	