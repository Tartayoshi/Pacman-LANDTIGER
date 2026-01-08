/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include "LPC17xx.h"
#include "timer.h"
#include "GLCD/GLCD.h" 
//#include "TouchPanel/TouchPanel.h"
#include <stdio.h> /*for sprintf*/
#include "Pacman/gameEngine.h"
#include "music/music.h"

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
//default 1.5
#define VOLUME 1.5 //from 0 to 1 value higher than 1 works, but sound is distorted

uint16_t SinTable[45] =                                       /* ÕýÏÒ±í                       */
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};


void TIMER0_IRQHandler (void) //1hz clock
{
		clock--;
	
		if(gameState == INGAME){
			updateTimeOut(clock); //Solve a case where clock was updated after GameOver screen
		}
	
		//if(clock == 0){ //handled in GameEngine update()
			//gameOver(1);
		//}

  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void TIMER1_IRQHandler (void) //note duration
{
	disable_timer(2);
	if(dotted == 1){ //check if the note is dotted "short note"
		reset_timer(1);
		init_timer(1, delay);
		enable_timer(1);
		dotted = 0;
	}else{
		notePlaying =  0;
	}
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

void TIMER2_IRQHandler (void) //Note Frequency
{
	static int sineticks=0;
	/* DAC management */	
	static int currentValue; 
	currentValue = SinTable[sineticks];
	currentValue *= VOLUME; 
	currentValue -= 410;
	currentValue /= 1;
	currentValue += 410;

	
	LPC_DAC->DACR = currentValue <<6;

	sineticks++;
	if(sineticks==45) sineticks=0;

	
  LPC_TIM2->IR = 1;			/* clear interrupt flag */
  return;
}

void TIMER3_IRQHandler (void){ //main loop
	update(); 									// Game Engine Handler
	reset_timer(3);
	if(ghostDeath == 1){
		disable_timer(3); // stop the game until the sound has been reproduced
	}else{
		enable_timer(3);
	}
	LPC_TIM3->IR = 1;			/* clear interrupt flag */
	return;
}



/******************************************************************************
**                            End Of File
******************************************************************************/
