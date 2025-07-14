// Author : Ethan Syed
// Will hold all necessary timer initializations

#ifndef __Unified_Timer_Init__
#define __Unified_Timer_Init__
#include <stdint.h>


// Clock Priority
#define Timer3A_Priority 2

//Btn Debounce
#define Timer0A_Period 6000000 //Will run every 25ms
#define Timer0A_Priority 4


void Unified_Timer_Init(void);

void timer3_Init(void);
void timer0_Init(void);


//Timer2A Methods + variables
// global
extern uint32_t SW0D;
extern uint32_t SW1D;
extern uint32_t SW2D;

void buttonDebounce(void);


#endif