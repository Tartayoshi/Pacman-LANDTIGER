//#include "/Bitmap/bitmap.h" 
#include "utility.h"
#include "map.h"
#include "bitmapData.h"
//#include "../Bitmap/bitmap.h" //included in player.h
#include "player.h"
#include "ghost.h"
#include "GLCD/GLCD.h"
#include "random/random.h"



//int pillCount; //should be 240
int superPillCount = 0; //handled by gameEngine
int pillPosition[MAXPILL]; //used for save the positin of existing pills

void drawWall(unsigned int x, unsigned int y, int mapIndex){ //draw the correct map tail in the correct position and trasformation,

	/******* awerness parameters *********/
	static int currentX = 0; //used for edge rotation
	static enum rotation Xrot = ROT90;
	static int currentY = 0;
	static enum rotation Yrot = MIRRORY;
	static char yTjunction = 0; //0 o 1
	//static int xTjunction = 0; //hold y of current y data
	/*************************************/
	
	
	char outerEdge = 1; //is it drawing an outer edge or an inner one?
	
	//*****************************outer edges******************************
	if(mapIndex == 0 ||
		mapIndex % mapDimX == 0 &&
		level0[mapIndex + 1] == WALL && 
		(mapIndex + mapDimX < mapDimX * mapDimY && level0[mapIndex + mapDimX] == WALL) !=
		(mapIndex - (int)mapDimX > 0 && level0[mapIndex - (int)mapDimX] == WALL)){  				//left corner
		if(y != currentY){
				currentY = y;
				if(Yrot == NOROT){
					Yrot = MIRRORY;
				}else{
					Yrot = NOROT;
				}
			}
		BITMAP_draw(mapOuterCorner, x, y, tailSize, tailSize, Yrot);
	}else if(mapIndex == mapDimX - 1 || 																								//right corner
					(mapIndex - (int)mapDimX + 1) % mapDimX == 0 &&
					level0[mapIndex - 1] == WALL &&
					(mapIndex + mapDimX < mapDimX * mapDimY && level0[mapIndex + mapDimX] == WALL) !=
					(mapIndex - (int)mapDimX > 0 && level0[mapIndex - (int)mapDimX] == WALL)){
				if(Yrot == MIRRORY){
					BITMAP_draw(mapOuterCorner, x, y, tailSize, tailSize, MIRRORYROT90);
				}else{
					BITMAP_draw(mapOuterCorner, x, y, tailSize, tailSize, MIRROR);
				}
			
	}else if(mapIndex < mapDimX || 																					//Edge Orizontal
					mapIndex > mapDimX * (mapDimY - 1) ||
					(mapIndex + mapDimX < mapDimX * mapDimY &&
					mapIndex - (int)mapDimX > 0 &&
					level0[mapIndex + mapDimX] != WALL && level0[mapIndex - mapDimX] != WALL)){
			if(y != currentY){
				currentY = y;
				if(Yrot == NOROT){
					Yrot = MIRRORY;
				}else{
					Yrot = NOROT;
				}
			}
			if((mapIndex + mapDimX < mapDimX * mapDimY && // handle t junctions
				level0[mapIndex + mapDimX] == WALL) ||
					(mapIndex - (int)mapDimX > 0 &&
						level0[mapIndex - mapDimX] == WALL)
					){
						if(yTjunction == 0){
								BITMAP_draw(mapOuterTJoint, x, y, tailSize, tailSize, Yrot);
								yTjunction = 1;
						}else{
							if(Yrot == NOROT){
								BITMAP_draw(mapOuterTJoint, x, y, tailSize, tailSize, MIRROR);
							}else{
								BITMAP_draw(mapOuterTJoint, x, y, tailSize, tailSize, MIRRORYROT90);
							}
							yTjunction = 0;
						}
				}else{
					BITMAP_draw(mapOuterEdge, x, y, tailSize, tailSize, Yrot);
				}
		
	}else if(mapIndex % mapDimX == 0 ||
					(mapIndex - (int)mapDimX + 1) % mapDimX == 0 ||
					mapIndex + 1 < mapDimX * mapDimY &&
					mapIndex - 1 > 0 && 
					level0[mapIndex + 1] != WALL && level0[mapIndex - 1] != WALL){ 												//Edges Veritcal
	
			if(Xrot == MIRRORROT90){
				Xrot = ROT90;
			}else{
				Xrot = MIRRORROT90;
			}
			if((mapIndex - (int)mapDimX + 1) % mapDimX != 0 && 																			// handle t junctions
				level0[mapIndex + 1] == WALL){
					if(level0[mapIndex + mapDimX + 1] != WALL){
						BITMAP_draw(mapOuterTJoint, x, y, tailSize, tailSize, MIRRORROT90);   //patch, tJunction not allining correctly on bottom left
					}else{
						BITMAP_draw(mapOuterTJoint2, x, y, tailSize, tailSize, NOROT);
					}
						
				}else if (mapIndex % mapDimX != 0 &&
						level0[mapIndex - 1] == WALL){
					if(level0[mapIndex + mapDimX - 1] != WALL){
						BITMAP_draw(mapOuterTJoint2, x, y, tailSize, tailSize, MIRRORYROT90); //patch, tJunction not allining correctly on bottom right
					}else{
						BITMAP_draw(mapOuterTJoint, x, y, tailSize, tailSize, ROT90);
					}
						
				
				}else{
					BITMAP_draw(mapOuterEdge, x, y, tailSize, tailSize, Xrot);
				}
			//BITMAP_draw(mapOuterEdge, x, y, tailSize, tailSize, Xrot); //end outeredges
	}else// *********************small inner corners **********************
																																														//left
	if(level0[mapIndex - 1] == WALL && level0[mapIndex + 1] != WALL &&
		((level0[mapIndex + mapDimX] == WALL && level0[mapIndex + mapDimX -1] != WALL)  !=
		(level0[mapIndex - mapDimX] == WALL && level0[mapIndex - mapDimX - 1] != WALL))){
			if(Yrot == MIRRORY){
				BITMAP_draw(mapInnerEdge, x, y, tailSize, tailSize, ROT90);
			}else{
				BITMAP_draw(mapInnerEdge, x, y, tailSize, tailSize, MIRRORYROT90);
			}
	}else 																																										//right
		if(level0[mapIndex + 1] == WALL && level0[mapIndex - 1] != WALL &&
		((level0[mapIndex + mapDimX] == WALL && level0[mapIndex + mapDimX + 1] != WALL)  !=
		(level0[mapIndex - mapDimX] == WALL && level0[mapIndex - mapDimX + 1] != WALL))){
			if(Yrot == MIRRORY){
				BITMAP_draw(mapInnerEdge, x, y, tailSize, tailSize, NOROT);
			}else{
				BITMAP_draw(mapInnerEdge, x, y, tailSize, tailSize, MIRRORY);
			}	
	}else{
		outerEdge = 0;
	}
	
	//*******************inner obstacle********************************
	
	if(outerEdge == 0){
		//left up corner
		if(level0[mapIndex + 1] == WALL &&
			level0[mapIndex - 1] != WALL &&
			level0[mapIndex + mapDimX] == WALL &&
			level0[mapIndex - mapDimX] != WALL){
				BITMAP_draw(obstacleCorner, x, y, tailSize, tailSize, NOROT);
			}else
			//right up corner
			if(level0[mapIndex + 1] != WALL &&
			level0[mapIndex - 1] == WALL &&
			level0[mapIndex + mapDimX] == WALL &&
			level0[mapIndex - mapDimX] != WALL){
				BITMAP_draw(obstacleCorner, x, y, tailSize, tailSize, MIRROR);
			}else 
			//left down corner
			if(level0[mapIndex + 1] == WALL &&
			level0[mapIndex - 1] != WALL &&
			level0[mapIndex + mapDimX] != WALL &&
			level0[mapIndex - mapDimX] == WALL){
				BITMAP_draw(obstacleCorner, x, y, tailSize, tailSize, MIRRORY);
			}else 
			//right down corner
			if(level0[mapIndex + 1] != WALL &&
			level0[mapIndex - 1] == WALL &&
			level0[mapIndex + mapDimX] != WALL &&
			level0[mapIndex - mapDimX] == WALL){
				BITMAP_draw(obstacleCorner, x, y, tailSize, tailSize, MIRRORYROT90);
			}else
			//orizontal up
			if(level0[mapIndex + 1] == WALL &&
			level0[mapIndex - 1] == WALL &&
			level0[mapIndex + mapDimX] == WALL &&
			level0[mapIndex - mapDimX] != WALL){
				BITMAP_draw(obstacleEdge, x, y, tailSize, tailSize, NOROT);
			}else
			//orizontal down
			if(level0[mapIndex + 1] == WALL &&
			level0[mapIndex - 1] == WALL &&
			level0[mapIndex + mapDimX] != WALL &&
			level0[mapIndex - mapDimX] == WALL){
				BITMAP_draw(obstacleEdge, x, y, tailSize, tailSize, MIRRORY);
			}else 
			//vertical left
			if(level0[mapIndex + 1] == WALL &&
			level0[mapIndex - 1] != WALL &&
			level0[mapIndex + mapDimX] == WALL &&
			level0[mapIndex - mapDimX] == WALL){
				BITMAP_draw(obstacleEdge, x, y, tailSize, tailSize, MIRRORROT90);
			}else 
			//vertical right
			if(level0[mapIndex + 1] != WALL &&
			level0[mapIndex - 1] == WALL &&
			level0[mapIndex + mapDimX] == WALL &&
			level0[mapIndex - mapDimX] == WALL){
				BITMAP_draw(obstacleEdge, x, y, tailSize, tailSize, ROT90);
			}else
			//inner left up
			if(level0[mapIndex + 1] == WALL &&
			level0[mapIndex - 1] == WALL &&
			level0[mapIndex + mapDimX] == WALL &&
			level0[mapIndex - mapDimX] == WALL &&
			level0[mapIndex - mapDimX -1] != WALL){
				BITMAP_draw(obstacleInnerCorner, x, y, tailSize, tailSize, MIRRORYROT90);
			}else
			//inner right up
			if(level0[mapIndex + 1] == WALL &&
			level0[mapIndex - 1] == WALL &&
			level0[mapIndex + mapDimX] == WALL &&
			level0[mapIndex - mapDimX] == WALL &&
			level0[mapIndex - mapDimX +1] != WALL){
				BITMAP_draw(obstacleInnerCorner, x, y, tailSize, tailSize, MIRRORY);
			}else
			//inner left down
			if(level0[mapIndex + 1] == WALL &&
			level0[mapIndex - 1] == WALL &&
			level0[mapIndex + mapDimX] == WALL &&
			level0[mapIndex - mapDimX] == WALL &&
			level0[mapIndex + mapDimX -1] != WALL){
				BITMAP_draw(obstacleInnerCorner, x, y, tailSize, tailSize, MIRROR);
			}else
			//inner right down
			if(level0[mapIndex + 1] == WALL &&
			level0[mapIndex - 1] == WALL &&
			level0[mapIndex + mapDimX] == WALL &&
			level0[mapIndex - mapDimX] == WALL &&
			level0[mapIndex + mapDimX + 1] != WALL){
				BITMAP_draw(obstacleInnerCorner, x, y, tailSize, tailSize, NOROT);
			}
	}
	
}

void drawSpawn(unsigned int x, unsigned int y){
	//**********DEBUG*********/ 
	//int i,j;
	//LCD_DrawLine(x, y, x + tailSize, y + tailSize, Red);
	/*************************/
	
	//could have used the tails, but i had still plenty of memory left, so I went lazy and printed the whole spawn as an immage
	BITMAP_draw(lazy_spawn, x + spawnX / 2, y + spawnY / 2, spawnX, spawnY, NOROT); 
	
}

void drawPill(unsigned int x, unsigned int y){
	//unsigned int startX = x;
	//int i,j;
	int i;
	//no need of a tail here, pils are literally 4px
	for(i = 0; i < pillSize; i++){
		LCD_DrawLine(x + offset, y + offset + i, x + offset + pillSize - 1, y + offset + i, 0xfdb5);
	}
	
	
}

void drawSuperPill(unsigned int x, unsigned int y){
	//unsigned int startX = x;
	//int i,j;
	BITMAP_draw(superPill,x, y, tailSize, tailSize, NOROT);
}

void drawMap(){ //check every tail in the matrix of tails and draw it on the screen

	int y = startYmap;
	int i, j, x;
	char spawnDrew = 0;
	
	offset = (tailSize - pillSize) / 2;
	
	
	for(i = 0; i < mapDimY; i++){
		x = startXmap;
		for(j = 0; j < mapDimX; j++){
			switch(level0[i*mapDimX + j]){
				case WALL:
					drawWall(x + tailSize / 2, y + tailSize / 2, i*mapDimX + j);
					break;
				case SPAW:
					if(spawnDrew == 0){ //it draw it once
						drawSpawn(x + tailSize / 2, y + tailSize / 2);
						spawnDrew = 1;
					}
					
					break;
				case PILL:
					drawPill(x, y);
					//pillCount++;
					break;
				case SUPE:
					drawSuperPill(x + tailSize / 2, y + tailSize / 2);
					break;
				case PACM: //set spawn pacman position
					player.spawnX = x + tailSize;
					player.spawnY = y + (tailSize / 2);
					player.SpawnMapIndex = i*mapDimX + j;
					//player.xIncrement = (int)tailSize / 2; //pacman spawn between two tails
					//player.yIncrement = 0;
					break;
				case BLIN:
					Blinky.spawnX = x + tailSize;
					Blinky.spawnY = y + (tailSize / 2);
					Blinky.SpawnMapIndex = i*mapDimX + j;
				break;
				default:
					break;
			}
			x += tailSize;
		}
		y += tailSize;
	}
	
}


//void resetTail(int index){
//	int x = index % mapDimX;
//	int y = index / mapDimX;
//	int i,j;
//	
//	if(level0[index] == SPAW || level0[index] == BLIN || level0[index] == PACM){
//		return;
//	}
//	
//	for(i = 0; i < tailSize; i++){
//		LCD_DrawLine(x * tailSize - tailSize / 2, i * tailSize, x * tailSize + tailSize / 2, i * tailSize, Black); 
//	}
//	/*
//	x *= tailSize + startXmap;
//	y *= tailSize + startYmap;
//	
//	switch(level0[index]){
//		case WALL:
//			drawWall(x, y, index); //need to be reworked to correctly funtioning
//			break;
//		case PILL:
//			drawPill(x, y);
//			break;
//		case SUPE:
//			drawSuperPill(x, y);
//			break;
//		default:
//			break;
//	}
//*/
//	drawMap();
//	
//}

void drawSuperPills(){

	int y = startYmap;
	int i, j, x;

	
	//reduced version of the draw map function
	for(i = 0; i < mapDimY; i++){
		x = startXmap;
		for(j = 0; j < mapDimX; j++){
			switch(level0[i*mapDimX + j]){
				
				case SUPE:
					drawSuperPill(x + tailSize / 2, y + tailSize / 2);
					break;

					break;
				default:
					break;
			}
			x += tailSize;
		}
		y += tailSize;
	}
	
}


void drawPills(){

	int y = startYmap;
	int i, j, x;

	
	//reduced version of the draw map function
	for(i = 0; i < mapDimY; i++){
		x = startXmap;
		for(j = 0; j < mapDimX; j++){
			switch(level0[i*mapDimX + j]){
				
				case PILL:
					drawPill(x, y);
					break;

					break;
				default:
					break;
			}
			x += tailSize;
		}
		y += tailSize;
	}
	
}

void placeSuperPill(){
	char count = 0; //count possible spawn points
	int i, j, randomI, res, X; 
	int Y = startYmap;
	//int volatile res;
	for(i = 0; i < mapDimX * mapDimY; i++){ //counting existing pills
		if(level0[i] == PILL){
			pillPosition[count++] = i;
		}
	}
	
	if(count == 0){
		return; //no pill to be found
	}
	
	res = random(count); // select a random pill from the existing ones

	randomI = pillPosition[res]; //select the index of the map matrix that rappresent the selected pill
	level0[randomI] = SUPE; // replace it with super pill
	
	//super pill will be displayed on next frame
	
}