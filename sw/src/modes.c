// Author : Ethan Syed

#include <stdint.h>
#include "./modes.h"
#include "./Unified_Timer_Init.h"
#include "Display.h"
#include "./globalVars.h"

// Modes : 3
// 0 - Default Mode 
// 1 - Compass Mode
// 2 - Exercise Mode

#define num_of_modes 3

uint8_t mode = 0;

void update_mode(void){
	mode++;
	mode = mode % num_of_modes;
	
	clear_screen();
	
	switch(mode){
		case 0:
			//default mode
			default_mode_init();
			break;
		case 1:
			//compass mode
			compass_mode_init();
			break;
		case 2:
			//exercise mode
			exercise_mode_init();
			break;
	}
	
}