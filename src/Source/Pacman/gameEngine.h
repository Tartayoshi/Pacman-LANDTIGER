#include "LPC17xx.h"
#include "player.h"
#include "map.h"
#include "gui.h"


extern void init();
//extern void testing();
extern volatile enum Direction joystick;
extern volatile char pauseButton;

enum GameStates{LOADING, READY, INGAME, PAUSE, GAMEOVER, ERROR};

extern enum GameStates gameState;


//extern void input(char);
extern void update();
extern int clock;
extern char musicHasPlayed;

extern void gameOver(char timeout);

/******* to sound engine *********/
extern char volatile ghostDeath;
extern char volatile pacmanDeath;