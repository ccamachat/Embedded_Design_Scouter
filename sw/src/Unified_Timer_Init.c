// Author : Ethan Syed
// Will hold all necessary timer initializations

#include <stdint.h>

#include "./Unified_Timer_Init.h"
#include "../inc/Timer0A.h"
#include "../inc/MQTT.h"
#include "./clock.h"
#include "./globalVars.h"

// Timer0A - Button Debounce
// Timer1A - XXX
// Timer2A - XXX
// Timer3A - Clock (Time)

void Unified_Timer_Init(void){
	startClock(Timer3A_Priority); // start clk
	timer0_Init();
}


//Individual Timer inits
void timer0_Init(void){
	Timer0A_Init(&buttonDebounce, Timer0A_Period, Timer0A_Priority);
}


/* ------------- Button Debounce Timer --------- */
// Variables
uint32_t SW0D = 2, SW1D = 2, SW2D = 2;

//Timer0A Methods
void buttonDebounce(void){
	SW0D == 2 ? 2 : SW0D++;
	SW1D == 2 ? 2 : SW1D++;
	SW2D == 2 ? 2 : SW2D++;
	
}