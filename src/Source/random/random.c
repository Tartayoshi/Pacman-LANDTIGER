#include "random.h"

#define DEFAULTSEED 0xAA
char seed = 0;
const unsigned char taps = 0x1D;

// Linear feedback shift register in c
char random(char max){
	
	char mask;
	char bitCounter = 0;
	
	if(seed == 0){
		seed = DEFAULTSEED;
	}
	
	mask = seed & taps;
	
	while(mask != 0){
		if(mask % 2 == 1){
			bitCounter++;
		}
		mask >>= 1;
	}
	
	seed >>= 1;
	if(bitCounter % 2 == 1){
		seed |= 0x80;
	}
	
	return seed % max;
	
}

