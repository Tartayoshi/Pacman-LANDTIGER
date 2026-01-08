#include "LPC17xx.h"
#include "utility.h"
#include "player.h"

#include "bitmapData.h"
#include "mapData.h"
//#include "mapData.h"
//#include "Bitmap/bitmap.h"
#ifndef GHOST_STRUCT
#define GHOST_STRUCT
enum GhostState {SPAWNING, CHASING, FLEEING, DEAD};
//enum GhostPath {VOID, PATH, TARG, CURR}; -> maptail used instead



struct Ghost{
	
	int spawnX;
	int spawnY;
	int SpawnMapIndex;
	int x;
	int y;
	int speed;
	
	enum Direction direction;
	//char moving; //bool
	unsigned int frame;
	uint16_t precFrame[BLINKYSIZE * BLINKYSIZE]; //color buffer for a snapshot of what is behind blinky
	char glitchedFrame; //chack if pacman has been tasted by the ghost, can happen sometimes
	unsigned int mapIndex; //reference to the position in the map
	
	char spawnTarget; //used for spawn animation
	
	enum GhostState state; //ghost state machine
	
	
	int coolDownTimer;
	//int score;
	//int lives;
	//int scoreForLife;
	
	int xIncrement; //used to eficently take note of player position in the map
	int yIncrement;
	
	enum PacTails navigator[MAPDIMX * MAPDIMY]; //PILL == PATH
	
//	/**** sound effects ****/ -> BLinky state is enought to determine blinky sounds
//	char moving;
//	char stuck;
	
		
} ;

extern struct Ghost Blinky;
extern void BlinkyHandler();
extern void drawBlinky();
#endif
