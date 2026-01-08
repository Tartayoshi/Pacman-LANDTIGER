#include "LPC17xx.h"
//#include "utility.h"
#include "Bitmap/bitmap.h"
#ifndef PLAYER_H
#define PLAYER_H
enum Direction{UP, DOWN, LEFT, RIGHT, NONE};

//struct state;
//struct Player{};
struct Player{
	
	int spawnX;
	int spawnY;
	int SpawnMapIndex;
	int x;
	int y;
	
	enum Direction direction;
	char moving; //bool
	unsigned int frame;
	unsigned int mapIndex; //reference to the position in the map
	
	int score;
	int lives;
	int scoreForLife;
	int eatenGhosts;
	
	int xIncrement; //used to eficently take note of player position in the map
	int yIncrement;
	
	/**** sound effects ****/
	char eatingPills; //send messages to rit for sound engine
	char lUP;
	
	
};

extern const uint16_t*  pac_death[]; //animation handled in gameEngine

extern struct Player player;
extern const int speedPlayer;
extern const int pillScore;
extern const int superPillScore;

extern void playerHandler();
extern void drawPlayer(enum rotation);
extern char collisionCheck(enum Direction);
extern void eatPill();
extern void addScore(int increment);
#endif