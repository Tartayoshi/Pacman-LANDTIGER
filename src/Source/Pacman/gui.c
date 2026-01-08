#include "gui.h"
//#include "LPC177x_8x.h"
#include <stdlib.h>
#include <stdio.h>
//#include <string.h>
#include "../GLCD/GLCD.h"
#include "bitmapData.h"
#include "../Bitmap/bitmap.h"

#include "utility.h"


#define LIVEOFFSET 16
#define TEXTOFFSET (8 + 4)

const uint16_t Y_SCORE_ROW1 = 8;
const uint16_t Y_SCORE_ROW2 = 8 + TEXTOFFSET;

const uint16_t X_TIMER_ROW1 = 8;
const uint16_t X_SCORE_ROW1 = 128;

const uint16_t X_TIMER_ROW2 = 50;
const uint16_t X_SCORE_ROW2 = 129;

const uint16_t Y_LIVES = 320 - 8;

const uint16_t X_PAUSE = 100;
const uint16_t Y_PAUSE = 156;

const uint16_t X_GAMEOVER = 85;
const uint16_t Y_GAMEOVER = 100;
//const uint16_t Y_GAMEOVER = 156;

const uint16_t X_VICTORY = 240 / 2;
const uint16_t Y_VICTORY = 320 / 2;

uint8_t TIMEOUT_TEXT[] = "GAME OVER IN";
uint8_t SCORE_TEXT[] = "SCORE";
uint8_t PAUSE_TEXT[] = "PAUSE";
uint8_t GAMEOVER_TEXT[] = "GAME OVER";


const uint16_t TEXT_COLOR = White;
const uint16_t TEXT_BACKGROUND_COLOR = Black;



void draw_static_text(){
	GUI_Text(X_TIMER_ROW1, Y_SCORE_ROW1, TIMEOUT_TEXT, TEXT_COLOR, TEXT_BACKGROUND_COLOR);
	//GUI_Text(50, 8 + 8 + 4, (uint8_t*)"60", White, Black); //placeholder
	
	GUI_Text(X_SCORE_ROW1, Y_SCORE_ROW1, SCORE_TEXT, TEXT_COLOR, TEXT_BACKGROUND_COLOR); //placeholder
	//GUI_Text(129, 8 + 8 + 4, (uint8_t*)"1000", White, Black);
}

void updateTimeOut(int seconds){
	uint8_t timerString[4]; //3 char + null
	//int to string conversion
	
	sprintf((char*)timerString, "%02d ", seconds);
	
	GUI_Text(X_TIMER_ROW2, Y_SCORE_ROW2, timerString, TEXT_COLOR, TEXT_BACKGROUND_COLOR);
}

void updateScore(int score){ //00xx
	
	uint8_t scoreString[5];
	
	sprintf((char*)scoreString, "%04d", score);
	
	GUI_Text(X_SCORE_ROW2, Y_SCORE_ROW2, scoreString, TEXT_COLOR, TEXT_BACKGROUND_COLOR);	
	
}

void updateLives(int lives){
	static int offset = LIVEOFFSET;
	//int i;
	if (lives == 0){
		return;
	}
	
	if(lives > 0){
		for(; lives > 0; lives--){
			BITMAP_draw(pac1, (uint16_t)offset, Y_LIVES, pacSize, pacSize, MIRROR);
			offset += LIVEOFFSET;
		}
	}else{
		for(; lives < 0; lives++){
			//BITMAP_draw(pac1del, (uint16_t)offset, Y_LIVES, pacSize, pacSize, MIRROR);
			//#todo
			offset -= LIVEOFFSET;
		}
	}
	
}

void drawPauseScreen(){
	uint16_t x,y;
	int counter = 0;
	//fade the screen to black
	for(y = 0; y < SCREENHIGHT; y++){
		for(x = 0; x < SCREENWIDTH; x++){
			if(counter % 2 == 0){
				LCD_SetPoint(x, y, TEXT_BACKGROUND_COLOR);
			}
			counter++;
		}
		counter++;
	}
	GUI_Text(X_PAUSE, Y_PAUSE, PAUSE_TEXT, TEXT_COLOR, TEXT_BACKGROUND_COLOR);
	
}

void deletePauseScreen(){
	
	GUI_Text(X_PAUSE, Y_PAUSE, PAUSE_TEXT, TEXT_BACKGROUND_COLOR, TEXT_BACKGROUND_COLOR);

	
}

void drawGameOverScreen(){
	uint16_t x,y;
	int counter = 0;
	//fade the screen to red
	for(y = 0; y < SCREENHIGHT; y++){
		for(x = 0; x < SCREENWIDTH; x++){
			if(counter % 2 == 0){
				LCD_SetPoint(x, y, Red);
			}
			//LCD_SetPoint(x, y, (Red + LCD_GetPoint(x, y))/2);
			counter++;
		}
		counter++;
	}
	GUI_Text(X_GAMEOVER, Y_GAMEOVER, GAMEOVER_TEXT, Blue, White);

	
}

void drawVictoryScreen(){
	
	BITMAP_draw(victoryScreen, X_VICTORY, Y_VICTORY, victoryScreenX, victoryScreenY, NOROT);
	
	
}

void writeReceveData(uint8_t rcvClock, uint8_t rcvLives, uint16_t rcvScore){ //data from CAN
	uint8_t string[32];
	
	GUI_Text(X_GAMEOVER, Y_GAMEOVER + TEXTOFFSET, (uint8_t*)"Receved Data:", TEXT_COLOR, TEXT_BACKGROUND_COLOR);
	
	sprintf((char*)string, "Time left: %d", rcvClock);
	GUI_Text(X_GAMEOVER, Y_GAMEOVER + TEXTOFFSET * 2, string, TEXT_COLOR, TEXT_BACKGROUND_COLOR); 
	
	sprintf((char*)string, "Lives: %d", rcvLives);
	GUI_Text(X_GAMEOVER, Y_GAMEOVER + TEXTOFFSET * 3, string, TEXT_COLOR, TEXT_BACKGROUND_COLOR); 
	
	sprintf((char*)string, "Score: %d", rcvScore);
	GUI_Text(X_GAMEOVER, Y_GAMEOVER + TEXTOFFSET * 4, string, TEXT_COLOR, TEXT_BACKGROUND_COLOR); 
}

void error(char * error){
	GUI_Text(8, Y_PAUSE, (uint8_t*)error, TEXT_COLOR, TEXT_BACKGROUND_COLOR);
}
