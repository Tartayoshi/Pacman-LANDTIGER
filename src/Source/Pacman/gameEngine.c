#include "utility.h"

#include "gameEngine.h"
#include "timer/timer.h"
#include "joystick/joystick.h"
#include "GLCD/GLCD.h" 
#include "RIT/RIT.h"
#include "button_EXINT/button.h"
#include "bitmapData.h"
#include "random/random.h"
#include "CAN/CAN.h"
#include "ghost.h"

/******** I M P O R T A N T ! ! ! **********/
//WARNING!!! the LCD_GetPoint function don't always work as expected,
//is highly dependent on the screen model of the board. Some issues has been addressed modifing
//GLCD library, but I'm not sure that I had every possible screen - board combination to try it on,
//so if you see any glitches on the screen try using a diffrent board


//#include "player.h"

/* Constant declaration */
//const speedPlayer = 5; //px
/************************/

volatile enum Direction joystick;
volatile char pauseButton;

int clock = CLOCKTIME; //sec
char musicHasPlayed = 0; //#TODO set to zero 
int nextSuperPillTime = -1;

/*****havie sound effect******/
char volatile ghostDeath;
char volatile pacmanDeath;
char pacmanDieAnimation;

void inputHandler(){
	
	static volatile enum Direction nextInput = NONE;
	
	if(joystick != NONE && joystick != player.direction){
		//player.direction = joystick;
		nextInput = joystick;
	}
	
	if(nextInput != NONE && collisionCheck(nextInput) == 0 && player.xIncrement == 0 && player.yIncrement == 0){
		player.direction = nextInput;
		player.moving = 1;
		nextInput = NONE;
		eatPill();
	}
	
	
}


//int cycle = 0;


/* Game State machine */

//Every state is divided in the single state loop function, 
//an enter function and an exit one if needed, everything is than linked in the
//game engine "update" function

enum GameStates gameState;
//states declaration

//GAMEOVER
void enterGameover(){
	CAN_Init();
	
	#ifdef SIMULATOR //simulate can bheivour
	writeReceveData(clock, player.lives, player.score);
	#endif
	
	CAN_TxMsg.data[0] = (uint8_t)clock;
	CAN_TxMsg.data[1] = (uint8_t)player.lives;
	CAN_TxMsg.data[2] = (player.score & 0xFF00 ) >> 8;
	CAN_TxMsg.data[3] = player.score & 0xFF;
	CAN_TxMsg.len = 4;
	CAN_TxMsg.id = 2;
	CAN_TxMsg.format = STANDARD_FORMAT;
	CAN_TxMsg.type = DATA_FRAME;
	CAN_wrMsg (1, &CAN_TxMsg);               /* transmit message */
	
	//writeReceveData(recvClock, recvLives, recvScore);
	
	

}

void gameOver(char timeout){
	gameState = GAMEOVER;
	disable_timer(0);
	if(timeout == 1){
			drawGameOverScreen();
	}else{
		drawVictoryScreen();
	}
	enterGameover();
}


//READY
void enterReady(){
	int i;
	for(i = 0; i < 16; i++){
		LCD_DrawLine(8, 8 + i, 240-8, 8 + i, Black); //delete loading text
	}
	draw_static_text(); //draw static ui
	updateTimeOut(clock); //draw current timer
	updateLives(player.lives);
	updateScore(player.score);
	
}
/*
void gameReady(){
	Moved in update
}
*/

//UTILITY
void reset(){
	
	/* Map and Player */
	drawMap();// set player spawn point and any releted variables
	//player.x = 120;
	//player.y = 160;
	player.frame = 0; //pacman animation frame
	player.direction = LEFT;
	player.moving = 1;
	//player.score = 0;
	player.lives = DEFAULTLIVES;
	player.scoreForLife = 0;
	player.x = player.spawnX;
	player.y = player.spawnY;
	player.xIncrement = (int)tailSize / 2;
	player.yIncrement = 0;
	player.mapIndex = player.SpawnMapIndex;
	
	//sound set
	player.eatingPills = 0;
	player.lUP = 0;
	ghostDeath = 0;
	pacmanDeath = 0;
	
	pacmanDieAnimation = 0;

	/* Ghost */
	Blinky.frame = 0; //pacman animation frame
	Blinky.direction = LEFT;
	//Blinky.moving = 1;
	//Blinky.score = 0;
	//Blinky.lives = DEFAULTLIVES;
	//Blinky.scoreForLife = 0;
	Blinky.x = Blinky.spawnX;
	Blinky.y = Blinky.spawnY;
	Blinky.speed = BLINKYSPEED1;
	Blinky.xIncrement = (int)tailSize / 2;
	Blinky.yIncrement = 0;
	Blinky.mapIndex = Blinky.SpawnMapIndex;
	Blinky.spawnTarget = 1;
	Blinky.glitchedFrame = 0;
	
	Blinky.state = SPAWNING;
	
	drawPlayer(NOROT); //Make player appear
	drawBlinky();
	
	gameState = READY; // switch to ready state
	enterReady();
	
}

//INGAME
void gameIngame(){ 
	
	//Blinky and Pacman interaction
	if(pacmanDeath == 1 && pacmanDieAnimation == 1){
		if(player.frame / 3 < 11){ // animation at a third of the speed
			//animation frame
			BITMAP_draw(pac_death[player.frame++ / 3], player.x, player.y, deathsize, deathsize, NOROT);
		}
		return;
		
	}else if(pacmanDeath == 0 && pacmanDieAnimation ==1){ //sound is over handler rest
		pacmanDieAnimation = 0;
		if(player.lives == 0){ //end
				gameOver(1);
				return;
			}
			
			LCD_Clear(Black);
			reset();
			return;
	}
		
	if(Blinky.mapIndex == player.mapIndex){//someone is goinig to die
		if(Blinky.state == CHASING){
			updateLives(-player.lives);
			player.lives--;
			player.frame = 0; // set up for animation
			
			pacmanDeath = 1;
			pacmanDieAnimation = 1;
			disable_timer(0); //pause for sound
			
			LCD_Clear(White); //flash screen
			LCD_Clear(Black);
			LCD_Clear(Red); //flash screen
			LCD_Clear(Black);
			
			draw_static_text(); //draw static ui
			updateTimeOut(clock); //draw current timer
			updateScore(player.score);
			updateLives(-player.lives);//this take te offset back to the starting position
			updateLives(player.lives);//this redraw all the lives
			drawMap();
			
			return;  //animation will start at the next call of timer3 handler
			
			//drawPlayer(NOROT);

			//if(player.lives == 0){ //end
			//	gameOver(1);
			//	return;
			//}
			
			//LCD_Clear(Black);
			//reset();
		}else
		if(Blinky.state == FLEEING){
			Blinky.state = DEAD;
			Blinky.coolDownTimer = 0; //handled when blinky is again at spawn
			player.eatenGhosts++;
			addScore(BLINKYSCORE);
			
			ghostDeath = 1;
			disable_timer(0); //pause for sound
			
			//LCD_Clear(White); //flash screen
			//LCD_Clear(Black);
			LCD_Clear(White); //flash screen
			LCD_Clear(Black);
			draw_static_text(); //draw static ui
			updateTimeOut(clock); //draw current timer
			updateScore(player.score);
			updateLives(-player.lives);//this take te offset back to the starting position
			updateLives(player.lives);//this redraw all the lives
			drawMap();
			drawPlayer(NOROT);
			
			//disable_timer(3); moved to timer
		}
	}
	
	
	inputHandler(); //handle input
	BlinkyHandler();
	playerHandler(); //handle and draw the player 
	
	
	//spawn superPill handler
	if(nextSuperPillTime >= clock && superPillCount < MAXSUPERPILL){

		placeSuperPill(); //place pill in random position

		seed += (player.score - player.xIncrement + player.yIncrement + player.mapIndex); //shuffle the seed
		nextSuperPillTime = random(clock / 2) + clock / 2;
		superPillCount++;
	}
	
	
	//blinky timer handler
	switch(Blinky.state){
		case SPAWNING:
		case CHASING:
			break;
		case FLEEING:
			if(Blinky.coolDownTimer >= clock){
				Blinky.state = CHASING;
			}
			break;
		case DEAD:
			if(Blinky.coolDownTimer >= clock){
				BITMAP_draw(Blinky.precFrame, Blinky.x, Blinky.y, BLINKYSIZE, BLINKYSIZE, NOROT); //reset spawn position
				Blinky.x = Blinky.spawnX;
				Blinky.y = Blinky.spawnY;
				Blinky.xIncrement = (int)tailSize / 2;
				Blinky.yIncrement = 0;
				Blinky.mapIndex = Blinky.SpawnMapIndex;
				Blinky.state = SPAWNING;
				drawBlinky();
			}
	}
		
}

//PAUSE
void enterPause(){
		
	disable_timer(0); //disable timer
	drawPauseScreen(); //draw pause screen
	
}

void exitPause(){
	
	deletePauseScreen();
	draw_static_text(); //draw static ui
	updateTimeOut(clock); //draw current timer
	updateScore(player.score);
	updateLives(-player.lives);//this take te offset back to the starting position
	updateLives(player.lives);//this redraw all the lives
	drawMap(); //redraw the map
	
	if(nextSuperPillTime == -1){ //game has just started, we have to set it up
		nextSuperPillTime = random(clock / 2) + clock / 2;
	}
	
	enable_timer(0); //restart timer
	gameState = INGAME; //change game state back to ingame
	
}

/*
void gamePause(){
	//unused
}
*/



/*********************/




//GameHandler
void update(){
	
	seed++; //touch the seed so its very difficult to predict
	switch(gameState){
		case LOADING:
			//initialization in function init()
			//do nothing
			break;
		
		case READY:

			if(musicHasPlayed == 1){
				
				gameState = PAUSE;//spec 7 extrpt 1
				enterPause();

			}
					
			break;
		
		case INGAME:
			//input for pause handled here, so every functrion is for sure declared before
		
			//GAME OVER?
			if(clock <= 0){
				gameOver(1); // 1 == for timeout
				break;
			}else if(player.score == (MAXPILL - superPillCount) * pillScore + superPillCount * superPillScore + player.eatenGhosts * BLINKYSCORE){
				gameOver(0); //0 == for winning
				break;
			}
			
			if(Blinky.state == CHASING && Blinky.speed == BLINKYSPEED1 && clock <= BLINKYFASTTIME){ // blinky more aggressive at the end
				Blinky.speed = BLINKYSPEED2;
			}else
			if(Blinky.state == FLEEING && Blinky.speed == BLINKYSPEED2){ // blinky cannot being catch by pacman on speed 2
				Blinky.speed = BLINKYSPEED1;
			}
			
			if(pauseButton == 1){
				pauseButton = 0;
				enterPause();
				gameState = PAUSE;
				break;
			}
			
			/*patch for superpills*/
			//pacman sometimes like to only have a taste of the superpill, without eating it, we dont' like leftovers so this snipet of code shuld fix this 
			drawSuperPills();
			/*********************/
			gameIngame();
			break;
		
		case PAUSE:
				
			//input for pause handled here
			if(pauseButton == 1){
				pauseButton = 0;
				exitPause();
				gameState = INGAME;
			}
		
			//gamePause();
			break;
		
		case GAMEOVER:
			//gameGameover();
			break;
		
		case ERROR:
			drawGameOverScreen();
			break;
	}
	
}


void init(){ //initialize every perifery
	
	//ADC_init();
	BUTTON_init();

	gameState = LOADING;
	LCD_Initialization();	
	joystick_init();	
	LCD_CalibrateGetPoint(); //workaround for some screens
	LCD_Clear(Black);
	
	GUI_Text(8, 8, (uint8_t*)"Loading...", White, Black);
	
	/* Timer */
	init_timer(3, 0x1312D0 / 2); //25ms timer for update function -> will start after the jingle has finished, handled in IRQ_RIT
	init_timer(0, 0x17D7840); //1hz timer (clock) 
	
	
	/* Input */
	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       	*/
	enable_RIT();													/* RIT enabled*/

	joystick = NONE;
	pauseButton = 0;
	
	player.score = 0;
	player.eatenGhosts = 0;
	reset();
	
	
}



