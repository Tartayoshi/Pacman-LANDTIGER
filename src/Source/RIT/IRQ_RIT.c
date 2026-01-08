/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "RIT.h"
#include "Pacman/gameEngine.h"
#include "music/music.h"
#include "music/musicData.h"
#include "timer/timer.h"
#include "Pacman/ghost.h"
//#include "../led/led.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

volatile int down=0;

void RIT_IRQHandler (void)
{					
	//a variable for every song - sound
	static int songNote = 0;
	static int gameoverNote = 0;
	static int wakaNote = 0;
	static int lives = 0;
	static int flee = 0;
	static int chase = 0;
	static int ghosteat = 0;
	static int pacmandie = 0;
	
	static int ticks = 0;

	
	/** sound handler **/
	switch(gameState){
		
		case READY:
				/** starting music **/
			if(musicHasPlayed == 0){
				
				if(songNote == (sizeof(song) / sizeof(song[0])))
					
				{
					if(!isNotePlaying()){
						musicHasPlayed = 1;
						enable_timer(3); //start main loop
					}
					break;
				}
				
				if(!isNotePlaying())
				{
					++ticks;
					if(ticks == UPTICKS)
					{
						ticks = 0;
						playNote(song[songNote++]);
					}
				}
				
				
			}
			break;
		case INGAME:
			/* sound effects */
			if(ghostDeath == 1){ //blinky eaten by pacman
				if(!isNotePlaying())
					{
						//player.lUP = 0;
						++ticks;
						if(ticks == UPTICKS)
						{
							ticks = 0;
							playNote(ghost_eaten[ghosteat++]);
						}
					}
					
					if(ghosteat == (sizeof(ghost_eaten) / sizeof(ghost_eaten[0])))
						
					{
						ghosteat = 0;
						ghostDeath = 0;
						enable_timer(3);
						enable_timer(0);
					}
				
			}else
			if(pacmanDeath == 1){ //Pacman is dead
				if(!isNotePlaying())
					{
						//player.lUP = 0;
						++ticks;
						if(ticks == UPTICKS)
						{
							ticks = 0;
							playNote(pacman_dies[pacmandie++]);
						}
					}
					
					if(pacmandie == (sizeof(pacman_dies) / sizeof(pacman_dies[0])))
						
					{
						pacmandie = 0;
						pacmanDeath = 0;
						enable_timer(3);
						enable_timer(0);
					}
				
			}else
			if(player.lUP == 1){ //priority 1, player earn a new life
				if(!isNotePlaying())
					{
						
						++ticks;
						if(ticks == UPTICKS)
						{
							ticks = 0;
							playNote(pacman_1up[lives++]);
						}
					}
					
					if(lives == (sizeof(pacman_1up) / sizeof(pacman_1up[0])))
						
					{
						lives = 0;
						player.lUP = 0;
					}
				
			}else 
			if(player.eatingPills == 1  && Blinky.state != FLEEING){ //priority 2 playear eat a pill (waka)
				if(!isNotePlaying())
					{
						
						++ticks;
						if(ticks == UPTICKS)
						{
							ticks = 0;
							playNote(pacman_eat[wakaNote++]);
						}
					}
					
					if(wakaNote == (sizeof(pacman_eat) / sizeof(pacman_eat[0])))
						
					{
						wakaNote = 0;
						player.eatingPills = 0;
					}
			}else
			if(Blinky.state == FLEEING){ //priority 2 blinky is eskaping the player!
				if(!isNotePlaying())
					{
						player.eatingPills = 0; //ignore eatPill message
						++ticks;
						if(ticks == UPTICKS)
						{
							ticks = 0;
							playNote(ghost_flee[flee++]);
						}
					}
					
					if(flee == (sizeof(ghost_flee) / sizeof(ghost_flee[0])))
						
					{
						flee = 0;
					}
			}else
			if(Blinky.state == CHASING){ //priority 3 //background blinky noise
				if(!isNotePlaying())
					{
						//player.eatingPills = 0;
						++ticks;
						if(ticks == UPTICKS)
						{
							ticks = 0;
							playNote(ghost_chase[chase++]);
						}
					}
					
					if(chase == (sizeof(ghost_chase) / sizeof(ghost_chase[0])))
						
					{
						chase = 0;
					}
			}
			
			break;
			
		case GAMEOVER:
			
			if(!isNotePlaying()) //play music untlill shutdown or restart of the board
				{
					++ticks;
					if(ticks == UPTICKS)
					{
						ticks = 0;
						playNote(mii[gameoverNote++]);
					}
				}
				
				if(gameoverNote == (sizeof(mii) / sizeof(mii[0])))
					
				{
					gameoverNote = 0;
				}
			
			
		default:
			break;
			
		
	}

	/* Joystick management */
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	
		joystick = UP;
	}else if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){
		joystick = RIGHT;
	}else if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){
		joystick = LEFT;
	}else if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){
		joystick = DOWN;
	}else{
		joystick = NONE;
	}
	
	/* button management */
	if(down != 0){
		down++;
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){
			switch(down){
				case 2:
					pauseButton = 1; //signal to gameEngine
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down=0;			
			NVIC_EnableIRQ(EINT0_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
		}
	} // end INT0
	
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
