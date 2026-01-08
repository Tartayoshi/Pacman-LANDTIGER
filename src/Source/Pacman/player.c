#include "player.h"
#include "../GLCD/GLCD.h" 
#include "bitmapData.h"
#include "mapData.h"
#include "gui.h"
#include "ghost.h"
#include "gameEngine.h"

#include "utility.h"


struct Player player;

/* Constant declaration */
const int speedPlayer = PLAYERSPEED; //px WARNING! speed need to be divisible by tailSize in order to work! (if you want speed difrent than 4 you need to rework the shadow)
const int pillScore = PILLSCORE;
const int superPillScore = SUPERPILLSCORE;
/************************/

const uint16_t* pac[3] = {pac0, pac1, pac2}; //all pacman animation frame
const uint16_t*  pac_death[11] = { //death animation
	 pac_death0,
	 pac_death1,
	 pac_death2,
	 pac_death3,
	 pac_death4,
	 pac_death5,
	 pac_death6,
	 pac_death7,
	 pac_death8,
	 pac_death9,
	 pac_death10,
};	

char collisionCheck(enum Direction direction){ //based on the direction of pacman chek if the next tail is a wall or not
	char isColliding = 0;
	
	switch(direction){
		case UP:
			if(level0[player.mapIndex - (int)mapDimX] == WALL || level0[player.mapIndex - (int)mapDimX] == SPAW){ //collision!!
				isColliding = 1;
			}
			break;
		case DOWN:
			if(level0[player.mapIndex + (int)mapDimX] == WALL || level0[player.mapIndex + (int)mapDimX] == SPAW){ //collision!!
				isColliding = 1;
			}
			break;
		case LEFT:
			if(player.mapIndex % mapDimX == 0){ //chek pacman effect
				isColliding = 0; //need to get pacman going
			}else if(level0[player.mapIndex - 1] == WALL || level0[player.mapIndex - 1] == SPAW){ //collision!!
				isColliding = 1;
			}
			break;
		case RIGHT:
			if((player.mapIndex - (int)mapDimX + 1) % mapDimX == 0){ //chek pacman effect
				isColliding = 0; //need to get pacman going
			}else if(level0[player.mapIndex + 1] == WALL || level0[player.mapIndex + 1] == SPAW){ //collision!!
				isColliding = 1;
			}
			break;
		default:
			break;
	}
	
	return isColliding;
}

void drawPlayer(enum rotation bitmapRot){ //call bitmapdraw for the current frame
	
	if(player.moving == 0){
				BITMAP_draw(pac[1], player.x, player.y, pacSize, pacSize, bitmapRot);
				player.frame = 1;
			}else{
		
				if(player.frame < 3){
					BITMAP_draw(pac[player.frame], player.x, player.y, pacSize, pacSize, bitmapRot);
				}else{
					BITMAP_draw(pac[4 - player.frame], player.x, player.y, pacSize, pacSize, bitmapRot);
				}
				player.frame++;
				if(player.frame == 4){
					player.frame = 0;
				}
			}
	
}

void addScore(int increment){
	player.score += increment;
	player.scoreForLife += increment;
	if(player.scoreForLife >= UP1){ //check if you have at least 1000 point for a new life
		player.scoreForLife -= UP1; //reset the life score counter
		player.lives++;
		updateLives(1); //draw an extra life on screen
		player.lUP = 1;
		
	}
	updateScore(player.score); //update the score counter
}

void eatPill(){ // Remove the pill and increment pacman score accordingly
	
	int increment;
	
	switch(level0[player.mapIndex]){ //look for what it have to eat
		case PILL:
			increment = pillScore;
			player.eatingPills = 1;
			break;
		case SUPE:
			increment = superPillScore;
			if(Blinky.state != DEAD && Blinky.state != SPAWNING){ //scare blinky
				Blinky.state = FLEEING;
				Blinky.coolDownTimer = clock - FLEETIME;
				//player.eatingPills = 0;
			}
			break;
		default:
			//sorry pacman, the pill is in another castle... I mean, tail!!
			return;
			
	}

	level0[player.mapIndex] = EMPT; //eat the pill
	addScore(increment);
}


void playerHandler(){ //core handler for the player, it moves it, draw its shadow and draw it
	
	int i; //loops
	int shadow_X; //used for pacmaneffect on shadow
	
	enum rotation bitmapRot; //used for draw the player
	
	switch(player.direction){ //handle every direction separately
		case UP:
			bitmapRot = MIRRORROT90; //step 1 select the transofrmation for the pacman "sprite"
		
			if(player.moving == 1){ //step 2, is it moving?
				
				for(i = 0; i < speedPlayer; i++){ //print shadow (for erasing pacman in the old position)
					LCD_DrawLine(player.x - pacSize / 2,
								player.y - i + pacSize / 2,
								player.x + pacSize / 2,
								player.y - i + pacSize / 2, Black);
					}
				
					player.y -= speedPlayer; //step 3 move the player on the screen
					player.yIncrement -= speedPlayer;
					
					if(player.yIncrement <= -(int)tailSize / 2){ //step 4 is pacman on the same tail?
						player.mapIndex -= mapDimX; 			//	step 5 move pacman in another tail
						player.yIncrement = (int)tailSize / 2; //reset the increment on the start of the new tail
						eatPill();  //step 6 eat eventual pills
						
					}else if(player.yIncrement == 0 && collisionCheck(UP) == 1){ //step 7 can pacman move again in the same direction ?
						player.moving = 0;
					}
				}
	
			
			break;
		
		case DOWN:
			bitmapRot = ROT90;
			if(player.moving == 1){
				
				for(i = 0; i < speedPlayer; i++){
					LCD_DrawLine(player.x - pacSize / 2,
								player.y + i - pacSize / 2,
								player.x + pacSize / 2,
								player.y + i - pacSize / 2, Black);
				}
				player.y += speedPlayer;
				player.yIncrement += speedPlayer;
				
				if(player.yIncrement >= (int)tailSize / 2){ //index update
					player.mapIndex += mapDimX;
					player.yIncrement = -(int)tailSize / 2;
					eatPill();
					
				}else if(player.yIncrement == 0 && collisionCheck(DOWN) == 1){
					player.moving = 0;
					
				}
			}
		
			
			break;
		
		case LEFT:
			
			bitmapRot = MIRROR;
			if(player.moving == 1){
				
				
				for(i = 0; i < speedPlayer; i++){
					/********** pacman effect on shadow **********/
					if(player.x - i + pacSize / 2 >=  mapDimX * tailSize + startXmap){ //pacmaneffect for the shadow
						shadow_X = player.x - mapDimX * tailSize - i + pacSize / 2;
					}else{
						shadow_X = player.x - i + pacSize / 2;
					}
					/********************************************/
					LCD_DrawLine(shadow_X,
								player.y - pacSize / 2,
								shadow_X,
								player.y + pacSize / 2, Black);
				}
				//pacman effect patch for the pacman effect (draw a black square outside the map)
				if((player.mapIndex - (int)mapDimX + 1) % mapDimX == 0 || player.mapIndex % mapDimX == 0){
					
					for(i = 0; i < SCREENW - (mapDimX * tailSize + startXmap); i++){
						LCD_DrawLine(mapDimX * tailSize + startXmap + i, player.y - pacSize / 2, mapDimX * tailSize + startXmap + i,  player.y + pacSize / 2, Black);
					}
					for(i = 0; i < startXmap; i++){
						LCD_DrawLine(0 + i, player.y - pacSize / 2, 0 + i,  player.y + pacSize / 2, Black);
					}
					
				}
				//*********************************************************************************
				
				player.x -= speedPlayer;
				player.xIncrement -= speedPlayer;
				
				/****pacman effect handler*********/
				if(player.x <= startXmap){ //PACMAN EFFECT!
					player.mapIndex += (mapDimX - 1); //take back the player
					player.x = mapDimX * tailSize + startXmap;
					player.xIncrement = (int)tailSize / 2;
					eatPill();
					/*******************************/
				}else	if(player.xIncrement <= -(int)tailSize / 2){
					player.mapIndex--;
					player.xIncrement = (int)tailSize / 2;
					eatPill();
					
				}else if(player.xIncrement == 0 && collisionCheck(LEFT) == 1){ //collision check
					player.moving = 0;
				}				
			}
			
			break;
		
		case RIGHT:
			bitmapRot = NOROT;
			if(player.moving == 1){
				
				for(i = 0; i < speedPlayer; i++){
					if(player.x + i - pacSize / 2 <  startXmap){ //pacmaneffect for the shadow
						shadow_X = player.x + mapDimX * tailSize + i - pacSize / 2;
					}else{
						shadow_X = player.x + i - pacSize / 2;
					}
					LCD_DrawLine(shadow_X,
								player.y - pacSize / 2,
								shadow_X,
								player.y + pacSize / 2, Black);
					}
				//pacman effect patch (draw a back square outside the map
				if((player.mapIndex - (int)mapDimX + 1) % mapDimX == 0 || player.mapIndex % mapDimX == 0){
					
					for(i = 0; i < SCREENW - (mapDimX * tailSize + startXmap); i++){
						LCD_DrawLine(mapDimX * tailSize + startXmap + i, player.y - pacSize / 2, mapDimX * tailSize + startXmap + i,  player.y + pacSize / 2, Black);
					}
					for(i = 0; i < startXmap; i++){
						LCD_DrawLine(0 + i, player.y - pacSize / 2, 0 + i,  player.y + pacSize / 2, Black);
					}
					
				}
					
				player.x += speedPlayer;
				player.xIncrement += speedPlayer;
				
				
				if(player.x >= mapDimX * tailSize + startXmap){ //PACMAN EFFECT!
					player.mapIndex -= (mapDimX - 1); //take back the player
					player.x = startXmap;
					player.xIncrement = -(int)tailSize / 2;
					eatPill();
					
				}else	if(player.xIncrement >= (int)tailSize  / 2){ //index update
					player.mapIndex++;
					player.xIncrement = -(int)tailSize / 2;
					eatPill();
				}else if(player.xIncrement == 0 && collisionCheck(RIGHT) == 1){ //collision check
						player.moving = 0;
				}
			}
					
			
			break;
				
		case NONE:
			//not used, the player is alwais facing a direction
			break;
		}
		
		drawPlayer(bitmapRot); //now draw the player
		
	
}

