#include "LPC17xx.h"
extern void draw_static_text();
extern void updateTimeOut(int seconds);
extern void updateScore(int score);
extern void drawPauseScreen();
extern void updateLives(int lives);
extern void deletePauseScreen();
extern void drawGameOverScreen();
extern void drawVictoryScreen();
extern void writeReceveData(uint8_t rcvClock, uint8_t rcvLives, uint16_t rcvScore);
extern void error(char * error);
