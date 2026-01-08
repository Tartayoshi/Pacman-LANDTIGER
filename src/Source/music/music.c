/****************************
		Library modified for
		 compatibility with
		Arduino Piezzo Music
*****************************/

#include "music.h"

#include "../timer/timer.h"

#define CLOCKF 25000000
#define SINSTEP 45


BOOL notePlaying = 0;
BOOL volatile dotted = 0;

unsigned int volatile delay; //used for dotted notes


void playNote(NOTE note)
{
	notePlaying =  TRUE;
	//unsigned int volatil;e test;
	if(note.freq != REST)
	{
		reset_timer(2);
		init_timer(2, (unsigned int)(CLOCKF / note.freq / SINSTEP));
		
		enable_timer(2);
	}
	reset_timer(1);
	if(note.duration < 0){ //negative duration == dotted notes
		init_timer(1, (unsigned int)(SECOND * SPEEDUP / (float)(-note.duration)*0.9 + 0.5));
		delay = (unsigned int)(SECOND * SPEEDUP / (float)(-note.duration)*0.1 + 0.5);
		dotted = 1;
	}else{
		init_timer(1, (unsigned int)(SECOND * SPEEDUP / (float)(note.duration) + 0.5));
		dotted = 0;
	}
	enable_timer(1);
}

BOOL isNotePlaying(void)
{
	//return ((LPC_TIM2->TCR != 0) || (LPC_TIM1->TCR != 0));
	return notePlaying;
}

