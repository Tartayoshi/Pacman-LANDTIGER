/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "Pacman/gameEngine.h"

//#include "timer/timer.h"

#ifdef SIMULATOR
#include "RIT/RIT.h"
#endif

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif

/******** I M P O R T A N T ! ! ! **********/
//WARNING!!! the LCD_GetPoint function don't always work as expected,
//is highly dependent on the screen model of the board. Some issues has been addressed modifing
//GLCD library, but I'm not sure that I had every possible screen - board combination to try it on,
//so if you see any glitches on the screen try using a diffrent board



int main(void)
{
  SystemInit();  												/* System Initialization (i.e., PLL)  */
	
	LPC_SC -> PCONP |= (1 << 22);  // TURN ON TIMER 2
	LPC_SC -> PCONP |= (1 << 23);  // TURN ON TIMER 3
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);	
	
	
	LPC_PINCON->PINSEL1 |= (1<<21);
	LPC_PINCON->PINSEL1 &= ~(1<<20);
	LPC_GPIO0->FIODIR |= (1<<26);
	
	
	init(); //GameEngine & LandTiger Initializzation
	
	
	//enable_timer(3);
	
	
	
  while (1)	
  {
		#ifdef SIMULATOR
		RIT_IRQHandler(); //The simulator like to ignore the rit
		#endif
		//update(); //GameEngine update function -> moved to timer 3
		__ASM("wfi");
		
		
  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
