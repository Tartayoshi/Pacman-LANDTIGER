#include "ghost.h"
#include "GLCD/GLCD.h" 
//#include "mapData.h"
//#include "player.h"


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>

#include "utility.h"
#include "gui.h"
#include "gameEngine.h"
#include "random/random.h"

#define FRAMES 2
//#define DEATHFRAMES 1
#define FLEEFRAMES 8

/******** I M P O R T A N T ! ! ! **********/
//WARNING!!! the LCD_GetPoint function don't always work as expected,
//is highly dependent on the screen model of the board. Some issues has been addressed modifing
//GLCD library, but I'm not sure that I had every possible screen - board combination to try it on,
//so if you see any glitches on the screen try using a diffrent board


struct Ghost Blinky;
/******* blinky frames *********************************************/
const uint16_t* blinky_l[FRAMES] = {blinky_l1, blinky_l2}; //blinky look left
const uint16_t* blinky_u[FRAMES] = {blinky_u1, blinky_u2}; //blinky look up
const uint16_t* blinky_d[FRAMES] = {blinky_d1, blinky_d2}; //blinky look down
const uint16_t* blinky_f[FRAMES] = {blinky_f1, blinky_f2}; //blinky is scared
const uint16_t* blinky_fe[FLEEFRAMES] = {blinky_f1, blinky_f2, blinky_f1, blinky_f2, blinky_f3, blinky_f4, blinky_f3, blinky_f4}; //blinky is scared and is about to start chasing again

//just a frame, blinky eyes
// const uint16_t  blinky_le []; 
// const uint16_t  blinky_ue [];
// const uint16_t  blinky_de[];
// const uint16_t  blinky_n[]; easter egg

/******* A * STRUCTURES *********/
typedef struct { 
	int x;
	int y;
} Point; //rappresentation of a point in the map array (is rappresented as a matrix)

typedef struct Node { //node of the graph for path fainding
    Point pos;
    int g_cost;
    int h_cost;
    int f_cost;
    struct Node* parent;
    bool inOpenList;
} Node;

int heuristic(Point p1, Point p2) { //Manatthan distance 

	return abs(p1.x - p2.x) + abs(p1.y - p2.y);

	
}
bool isValid(int x, int y) { //is the point inside the map?
	return x >= 0 && x < MAPDIMX && y >= 0 && y < MAPDIMY;
}

Node* findLowestFcost(Node nodes[], int mapSize) {
    static Node* openList[MAPDIMX * MAPDIMY]; // Static open list
    int openListSize = 0;

    for (int i = 0; i < mapSize; i++) {
        if (nodes[i].inOpenList) {
            openList[openListSize++] = &nodes[i];
        }
    }

    if (openListSize == 0) return NULL;

    Node* lowest = openList[0];
    for (int i = 1; i < openListSize; i++) {
        if (openList[i]->f_cost < lowest->f_cost) {
            lowest = openList[i];
        }
    }
    return lowest;
}
/**********************************************/
void aStar() { //A* pathfind algorithm
		//static bool left = true; //used for spawn
    static Node nodes[MAPDIMX * MAPDIMY]; //static allocation of memory for the algoritmh,
		Point start, target;
		//Point originalTarget;
		//Point farthestTarget = {-1, -1}; //inverted euristic
		//int maxDistance = INT_MIN; //inverted euristic
		
		int x, y, i, xf, yf;
    for (y = 0; y < MAPDIMY; y++) { //initialization of alghoritm
        for (x = 0; x < MAPDIMX; x++) {
					//nodes set up
            nodes[y * MAPDIMX + x].pos.x = x;
            nodes[y * MAPDIMX + x].pos.y = y;
            nodes[y * MAPDIMX + x].g_cost = INT_MAX;
            nodes[y * MAPDIMX + x].h_cost = 0;
            nodes[y * MAPDIMX + x].f_cost = INT_MAX;
            nodes[y * MAPDIMX + x].parent = NULL;
            nodes[y * MAPDIMX + x].inOpenList = false;
					
					//copy matrix in blinky navigator
						switch (level0[y * MAPDIMX + x]){
							case WALL:
							case SPAW:
								Blinky.navigator[y * MAPDIMX + x] = WALL;
								break;
							case PILL:
							case SUPE:
							case EMPT:
								if(x == 0 || x == MAPDIMX - 1){ //close tunnels
									Blinky.navigator[y * MAPDIMX + x] = WALL;
									break;
								}
							case BLIN: //spawn pos of ghost
							case PACM: //spawn pos of pacman
								Blinky.navigator[y * MAPDIMX + x] = EMPT;
								break;
						
						}
						if(y * MAPDIMX + x == Blinky.mapIndex){ //set start where blinky is
							start.x = x;
							start.y = y;
							Blinky.navigator[Blinky.mapIndex] = BLIN;
							
						}
						switch(Blinky.state){
							case SPAWNING:
								return;//should neve happen
							case CHASING:
							case FLEEING:
								if(y * MAPDIMX + x == player.mapIndex){	//set target, is the same for chasing and fleeing, is just used diffrently
							
									target.x = x;
									target.y = y;
									Blinky.navigator[player.mapIndex] = PACM;
								}
								break;
							case DEAD:
								if(y * MAPDIMX + x == Blinky.SpawnMapIndex){	//set target
								
									if(Blinky.coolDownTimer == 0){
										target.x = x;
										target.y = y;
										Blinky.navigator[Blinky.SpawnMapIndex] = PACM; //back home
									}
									
									
								}
								break;
						}
							
        }
    }
		
		//Opening spawn doors for Blinky
		Blinky.navigator[Blinky.SpawnMapIndex - MAPDIMX * 2] = EMPT; //create entrace for spawn
		Blinky.navigator[Blinky.SpawnMapIndex - MAPDIMX * 2 + 1] = EMPT; //create entrace for spawn

    Node* startNode = &nodes[start.y * MAPDIMX + start.x];
    startNode->g_cost = 0;
    startNode->h_cost = heuristic(start, target);
    startNode->f_cost = startNode->h_cost;
    startNode->inOpenList = true;

    while (true) { 
        Node* current = findLowestFcost(nodes, MAPDIMX * MAPDIMY);
        if (current == NULL) { 
					return; //Path not found: 404
				}
        current->inOpenList = false;

        if (current->pos.x == target.x && current->pos.y == target.y) {
            //printf("Path found!\n");
            Node* path = current;
            while (path != NULL) {
                Blinky.navigator[path->pos.y * MAPDIMX + path->pos.x] = PILL;
                path = path->parent;
                if(path != NULL && path->pos.x == start.x && path->pos.y == start.y) break;
            }
            //Blinky.navigator[start.y * MAPDIMX + start.x] = BLIN;

            return;
        }

        int dx[] = {0, 0, 1, -1};
        int dy[] = {1, -1, 0, 0};
        for (i = 0; i < 4; i++) {
						int nx = current->pos.x + dx[i]; // Calculate neighbor coordinates
            int ny = current->pos.y + dy[i];
            //Point neighborPos = {current->pos.x + dx[i], current->pos.y + dy[i]};
            if (isValid(nx, ny) && Blinky.navigator[ny * MAPDIMX + nx] != WALL) { // Use calculated coordinates
                Node* neighbor = &nodes[ny * MAPDIMX + nx]; // Access node using correct indices
                if (neighbor == NULL) continue;
                int tentative_g_cost = current->g_cost + 1;

                if (tentative_g_cost < neighbor->g_cost) {
                    neighbor->g_cost = tentative_g_cost;
                    neighbor->h_cost = heuristic((Point){nx, ny}, target); // Use calculated coordinates
                    neighbor->f_cost = neighbor->g_cost + neighbor->h_cost;
                    neighbor->parent = current;
                    neighbor->inOpenList = true;
                }
            }
        }
    }
}




void drawBlinky(){
	uint16_t** currentBlinky;
	uint16_t pixel;
	int bx, by;
	int i = 0; //frameBuffer pos
	
	//before drawing the ghost we need to save the background
	for(by = Blinky.y - blinkySize/2; by < Blinky.y + blinkySize/2; by++){
		for(bx = Blinky.x - blinkySize/2; bx < Blinky.x + blinkySize/2; bx++){
			
			/******** I M P O R T A N T ! ! ! **********/
			/***** may cause glith on some board ****/
			//WARNING!!! the LCD_GetPoint function don't always work as expected,
			//is highly dependent on the screen model of the board. Some issues has been addressed modifing
			//GLCD library, but I'm not sure that I had every possible screen - board combination to try it on,
			//so if you see any glitches on the screen try using a diffrent board
			

			pixel = LCD_GetPoint(bx, by); // <--------------------- this
			/*****************************/			
			
			if(pixel == 0xffe0){ //look if pacman was here (0xffe0) is the color of pacman
				Blinky.glitchedFrame = 1;  //buffer will be erased and set to black 
				//pixel = Black; // yes, but it wont solve issues with pills, if pacman eat blinky and pills at same time a ghost of those pills will be present
			}
			Blinky.precFrame[i++] = pixel;
		}
	}	
	
	switch (Blinky.state){ //draw the correct "sprite" for Blinky
		case SPAWNING:
		case CHASING:
			if (Blinky.frame >= FRAMES){
				Blinky.frame = 0;
			}
			if(Blinky.direction == UP){
				BITMAP_draw(blinky_u[Blinky.frame], Blinky.x, Blinky.y, BLINKYSIZE, BLINKYSIZE, NOROT);
			}else
			if(Blinky.direction == DOWN){
				BITMAP_draw(blinky_d[Blinky.frame], Blinky.x, Blinky.y, BLINKYSIZE, BLINKYSIZE, NOROT);
			}else
			if(Blinky.direction == LEFT){
				BITMAP_draw(blinky_l[Blinky.frame], Blinky.x, Blinky.y, BLINKYSIZE, BLINKYSIZE, NOROT);
			}else
			if(Blinky.direction == RIGHT){
				BITMAP_draw(blinky_l[Blinky.frame], Blinky.x, Blinky.y, BLINKYSIZE, BLINKYSIZE, MIRROR);
			}else{
				BITMAP_draw(blinky_l[1], Blinky.x, Blinky.y, BLINKYSIZE, BLINKYSIZE, NOROT);
			}
			break;
		case FLEEING:
			if(clock - Blinky.coolDownTimer > 3){
				if (Blinky.frame >= FRAMES){
						Blinky.frame = 0;
					}
					if(Blinky.direction == NONE){
						BITMAP_draw(blinky_n, Blinky.x, Blinky.y, BLINKYSIZE, BLINKYSIZE, NOROT); //easter egg
					}else{
						BITMAP_draw(blinky_f[Blinky.frame], Blinky.x, Blinky.y, BLINKYSIZE, BLINKYSIZE, NOROT); //Blue blinky
					}
				}else{
					if (Blinky.frame >= FLEEFRAMES){
						Blinky.frame = 0;
					}
					if(Blinky.direction == NONE){
						BITMAP_draw(blinky_fe[FLEEFRAMES - 1], Blinky.x, Blinky.y, BLINKYSIZE, BLINKYSIZE, NOROT);
					}else{
						BITMAP_draw(blinky_fe[Blinky.frame], Blinky.x, Blinky.y, BLINKYSIZE, BLINKYSIZE, NOROT); //Blue blinking Blinky
					}
				}
			break;
		case DEAD:
			if(Blinky.direction == UP){
				BITMAP_draw(blinky_ue, Blinky.x, Blinky.y, BLINKYSIZE, BLINKYSIZE, NOROT);
			}else
			if(Blinky.direction == DOWN){
				BITMAP_draw(blinky_de, Blinky.x, Blinky.y, BLINKYSIZE, BLINKYSIZE, NOROT);
			}else
			if(Blinky.direction == LEFT){
				BITMAP_draw(blinky_le, Blinky.x, Blinky.y, BLINKYSIZE, BLINKYSIZE, NOROT);
			}else
			if(Blinky.direction == RIGHT){
				BITMAP_draw(blinky_le, Blinky.x, Blinky.y, BLINKYSIZE, BLINKYSIZE, MIRROR);
			}
			break;
	}
	
	
	Blinky.frame++;
	
}

void moveBlinky(){
	switch(Blinky.direction){ //handle every direction separately, handled like the player but blinky can't go to tunnels in this version
		case UP:

			Blinky.y -= Blinky.speed; //step 3 move the player on the screen
			Blinky.yIncrement -= Blinky.speed;
			
			if(Blinky.yIncrement <= -(int)tailSize / 2){ //step 4 is pacman on the same tail?
				Blinky.navigator[Blinky.mapIndex] = EMPT; //delete visited tail
				//resetTail(Blinky.mapIndex);
				Blinky.mapIndex -= mapDimX; 			//	step 5 move pacman in another tail
				Blinky.yIncrement = (int)tailSize / 2; //reset the increment on the start of the new tail
				Blinky.navigator[Blinky.mapIndex] = BLIN; //set new position
				//eatPill();  //step 6 eat eventual pills

			}
		

			
			break;
		
		case DOWN:
				Blinky.y += Blinky.speed;
				Blinky.yIncrement += Blinky.speed;
				
				if(Blinky.yIncrement >= (int)tailSize / 2){ //index update
					Blinky.navigator[Blinky.mapIndex] = EMPT;
					Blinky.mapIndex += mapDimX;
					Blinky.yIncrement = -(int)tailSize / 2;
					Blinky.navigator[Blinky.mapIndex] = BLIN; //set new position
					//eatPill();
				
				}				
			
			break;
		
		case LEFT:
			
			Blinky.x -= Blinky.speed;
			Blinky.xIncrement -= Blinky.speed;

			if(Blinky.xIncrement <= -(int)tailSize / 2){
				Blinky.navigator[Blinky.mapIndex] = EMPT;
				Blinky.mapIndex--;
				Blinky.xIncrement = (int)tailSize / 2;
				Blinky.navigator[Blinky.mapIndex] = BLIN; //set new position
					//eatPill();

				
			}			
		
		
			break;
		
		case RIGHT:
				
			
			Blinky.x += Blinky.speed;
			Blinky.xIncrement += Blinky.speed;
		
			if(Blinky.xIncrement >= (int)tailSize  / 2){ //index update
				Blinky.navigator[Blinky.mapIndex] = EMPT;
				Blinky.mapIndex++;
				Blinky.xIncrement = -(int)tailSize / 2;
				Blinky.navigator[Blinky.mapIndex] = BLIN; //set new position
				//eatPill();

			}
					
			
			break;
				
		case NONE:
			//no movment here, is still
			break;
		}
		
}

//handler
void BlinkyHandler(){
	
	static enum Direction awayFromPacman[3]; //used for flee
	int i, j, sx, sy;
	//if(Blinky.mapIndex != player.mapIndex){
		if(Blinky.glitchedFrame == 1 && //optimazed case blinky taste a pice of pacman
			Blinky.mapIndex != player.mapIndex &&
			Blinky.mapIndex != player.mapIndex + 1 &&
			Blinky.mapIndex != player.mapIndex - 1 &&
			Blinky.mapIndex != player.mapIndex + MAPDIMX &&
			Blinky.mapIndex != player.mapIndex - MAPDIMX
		){
			//drawMap();					//Warning!! it cause lag!
			drawPills();					//Ok, is not lagging anymore
			Blinky.glitchedFrame = 0;
			for(i = 0; i < BLINKYSIZE * BLINKYSIZE; i++){
				Blinky.precFrame[i] = 0x0000; //set backround at zero, solve bug phantom pills when pacman eat blinky and pill at the same time
			}
	}
	BITMAP_draw(Blinky.precFrame, Blinky.x, Blinky.y, BLINKYSIZE, BLINKYSIZE, NOROT); //draw precedent frame

	switch(Blinky.state){
		
		case SPAWNING:
			//BITMAP_draw(Blinky.precFrame, Blinky.x, Blinky.y, BLINKYSIZE, BLINKYSIZE, NOROT); //drawBlinky background
					
			if(Blinky.mapIndex != Blinky.SpawnMapIndex - MAPDIMX * 3 || Blinky.yIncrement != 0){ 
				Blinky.direction = UP; //raise blinky from the spawn position
				Blinky.y -= Blinky.speed;
				Blinky.yIncrement -= Blinky.speed;
				if(Blinky.yIncrement <= -(int)tailSize / 2){ //step 4 is pacman on the same tail?
						Blinky.mapIndex -= mapDimX; 			//	step 5 move pacman in another tail
						Blinky.yIncrement = (int)tailSize / 2; //reset the increment on the start of the new tail
						
					}
			}else{ //Blinky is out, chase!
				Blinky.state = CHASING;
				Blinky.direction = LEFT;//solve a rare bug, if pacman eat a SuperPill while blinky is rising up blinky won't stop raising up untill it reach new horizons,
				//so at the start it will go left by default and correct on the next call od aStar.
			}
			break;
		case FLEEING:
			//Lutharon flee-mode
			//Blink will look at the path to reach pacman and randomly select "in panic" a diffrent avaible one
			if(Blinky.yIncrement == 0 && Blinky.xIncrement == 0){ 
				aStar(); //calculate path to pacman
				i = 0; //avaible direction counter
				if(/*Blinky.xIncrement == 0 && */Blinky.navigator[Blinky.mapIndex - MAPDIMX] == EMPT){ //look up
					awayFromPacman[i++] = UP;
				}
				if(/*Blinky.xIncrement == 0 && */Blinky.navigator[Blinky.mapIndex + MAPDIMX] == EMPT){ //look down
					awayFromPacman[i++] = DOWN;
				}
				if(/*Blinky.yIncrement == 0 && */Blinky.navigator[Blinky.mapIndex + 1] == EMPT){ //look right
					awayFromPacman[i++] = RIGHT;
				}
				if(/*Blinky.yIncrement == 0 && */Blinky.navigator[Blinky.mapIndex - 1] == EMPT){ //look right
					awayFromPacman[i++] = LEFT;
				}
				
				if(i == 0){ //there is no escape, just accept the fate
					Blinky.direction = NONE;
					break;
				}
				seed += (Blinky.x + Blinky.y); //touch the random parameter seed
				Blinky.direction = awayFromPacman[random((char)i)];
				
				
			}
			moveBlinky();
			break;
			
		case CHASING:
		case DEAD:
			//aStar(); //calculate path to pacman
		if(Blinky.yIncrement == 0 && Blinky.xIncrement == 0){ //&& and not || couse of a few bug:
																													//1 blinky ascending when spawnig ad pacman eat a superpill;
																													//2  Blinky take sharp corner and decentrate from the map tails
		//set new route to navigator:
				aStar(); //calculate path to pacman
				if(/*Blinky.yIncrement == 0 &&*/ Blinky.direction != LEFT && Blinky.navigator[Blinky.mapIndex - 1] == PILL){
					Blinky.direction = LEFT;
				} else 
				if(/*Blinky.yIncrement == 0 &&*/ Blinky.direction != RIGHT && Blinky.navigator[Blinky.mapIndex + 1] == PILL){
					Blinky.direction = RIGHT;
				}else
				if(/*Blinky.xIncrement == 0 &&*/ Blinky.direction != UP && Blinky.navigator[Blinky.mapIndex - MAPDIMX] == PILL){
					Blinky.direction = UP;
				}else
				if(/*Blinky.xIncrement == 0 &&*/ Blinky.direction != DOWN && Blinky.navigator[Blinky.mapIndex + MAPDIMX] == PILL){
					Blinky.direction = DOWN;
				}else if(Blinky.yIncrement == 0 && Blinky.xIncrement == 0 &&    //path not found
						Blinky.navigator[Blinky.mapIndex + MAPDIMX] != PILL		&&
						Blinky.navigator[Blinky.mapIndex - MAPDIMX] != PILL		&&
						Blinky.navigator[Blinky.mapIndex - 1] != PILL	 &&
						Blinky.navigator[Blinky.mapIndex + 1] != PILL	
				){ 
					Blinky.direction = NONE; //practically impossible
				}
			}
		
		if(Blinky.mapIndex == Blinky.SpawnMapIndex && Blinky.coolDownTimer == 0){ //blinky is dead, set timer
			Blinky.coolDownTimer = clock - SPAWNCOOLDOWN; //#todo sni
			Blinky.spawnTarget = 1;
			Blinky.direction = RIGHT;
		}else
		if(Blinky.state == DEAD && Blinky.mapIndex == Blinky.SpawnMapIndex + 3){ //target 1 = right
			Blinky.spawnTarget = 0;
			Blinky.direction = LEFT;
		}else
		if(Blinky.state == DEAD && Blinky.mapIndex == Blinky.SpawnMapIndex - 2){
			Blinky.spawnTarget = 1;
			Blinky.direction = RIGHT;
		}
		moveBlinky();
		break;
				
	}
		

			
	
	drawBlinky();
	
}







