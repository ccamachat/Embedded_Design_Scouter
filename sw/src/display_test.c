// Author : Ethan S

#include "../src/globalVars.h"
#include "../src/globalFunctions.h"
#include "../src/Display.h"
#include "../inc/ST7735.h"
#include "../inc/CortexM.h"
#include "../src/modes.h"
#include "../src/Buttons.h"
#include "../src//Unified_Timer_Init.h"

#if MAIN == DISPLAY
volatile uint32_t changeModeFlag = 0;
volatile uint32_t refreshDefaultFlag = 0;
volatile uint32_t refreshCompassFlag = 0;
volatile uint32_t refreshExerciseFlag = 0;

int main(void){
	DisableInterrupts();
	Unified_Timer_Init();				// Timer init
	Button_Init();							// Button Init
	ST7735_InitR(INITR_REDTAB); // LCD Init
	display_init();							// Display init (rotate)
	start_sequence();						// Greeting Message
	DelayWait10ms(20);
	clear_screen();
	default_mode_init();				// Default hudview
	//DelayWait10ms(20);
	//clear_screen();
	//compass_mode_init();
	//exercise_mode_init();
	EnableInterrupts();
	while(1){
		WaitForInterrupt();
		if (changeModeFlag) {
			update_mode();
			changeModeFlag = 0;
		}
		if(refreshDefaultFlag){
			refreshDefaultFlag = 0;
		}
		if(refreshCompassFlag){
			refreshCompassFlag = 0;
		}
		if(refreshExerciseFlag){
			refreshExerciseFlag = 0;
		}
	}
	
}
#endif