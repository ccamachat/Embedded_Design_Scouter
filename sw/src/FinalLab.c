/**
 * @file FinalLab.c
 * @author Neilan Garcia (neilantgarcia@utexas.edu), Phebe Tan(phebe.tan@austin.utexas.edu), 
	Coty Camachat(crcamachat@utexas.edu), Ethan Syed(ethansyed@utexas.edu)
 *    <Aeybel Varghese, 16990>
 *    Good luck!
 * 
 * @version 0.1
 * @date 2024-02-22 <REPLACE WITH DATE OF LAST REVISION>
 *
 * @copyright Copyright (c) 2022
 *
 *    Warning. Initial code for the RGB driver creates bright flashing lights. Remove this code
 *    and do not run if you have epilepsy.
 */

/** File includes. */
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "../inc/Timer1A.h"
#include "Sound.h"


#include "../inc/tm4c123gh6pm.h" // TM4C register definitions
#include "../inc/PLL.h" // Clocking
#include "../inc/CortexM.h" // Clock delay & interrupt control
#include "../inc/Valvano_Unified_Port_Init.h" // Valvano initialization of all pins
#include "../inc/UART.h" // UART
#include "../inc/ST7735.h" // ST7735 display
#include "../lib/RGB/RGB.h"

/* NOTE: We suggest using the ./inc/ADCSWTrigger.h and the ./inc/TimerXA.h headers. */
#include "../inc/DAC.h"
#include "../inc/Dump.h"

/* Our Sensors */
#include "MAX30102_driver.h"
#include "LSM303A_driver.h"

#include "globalVars.h" 					// Global Variables
#include "globalFunctions.h"			// Global Functions
#include "Unified_Port_Init.h" 		// Unified Component Intitializations
#include "Unified_Timer_Init.h"		// Unified Timer Initializations
#include "Display.h"							// Display initialization
#include "modes.h"								// Handle mode switching

/** MMAP Pin definitions. */
#define PF0   (*((volatile uint32_t *)0x40025004)) // Left Button
#define PF1   (*((volatile uint32_t *)0x40025008)) // RED LED
#define PF2   (*((volatile uint32_t *)0x40025010)) // BLUE LED
#define PF3   (*((volatile uint32_t *)0x40025020)) // GREEN LED
#define PF4   (*((volatile uint32_t *)0x40025040)) // Right Button

/*	Function Prototypes		*/
void Pause(void);
void TimerInt(void);
void changeNote(void);
//int notepntr = 0;
//int pntr = 0;

#if MAIN == FINAL

// Flag initializations
volatile uint32_t changeModeFlag, updateStepFlag, updateHeartrateFlag;

int main(void) {
    DisableInterrupts();
    /* Interrupts currently being used:
       Timer0A (pri4) - Button Debouncing (DO NOT USE RGB INIT)
			 Timer1A - Clock running (Local tick)
			 Timer2A -
			 Timer3A (priX) - Heartbeat polling
	
       UART0, pri7 - PC communication
    */
	
		/*	Initializations		*/
    PLL_Init(Bus80MHz); // PLL Init
		UART_Init(); // UART Input Init (Baud rate : 115200)
		dac_init(); // Dac Init
		Unified_Port_Init(); // All of *our* Port + Component Initializations
	
    //RGBInit(); // RGB Flashing
		// Abstract the following sound generation later 
		//Timer0A_Init(&TimerInt, C3, 2); 	//timer0A produces the sound CANNOT USE TIMER0A
		//Timer1A_Init(&changeNote, 80000000,2); //timer 1A plays the actual note/changes the note
    EnableInterrupts();

		/*	Main Code		*/
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_SetCursor(0, 0);
    ST7735_OutString(
        "ECE445L Final Lab \n"
        "Press SW1 to start.\n");
    UART_OutString(
        "ECE445L Lab 10 \r\n"
        "Press SW1 to start.\r\n");
    Pause();
		
		while(1){
			WaitForInterrupt();

			if(changeModeFlag){
				changeModeFlag = 0;
			}
			if (updateStepFlag){
				updateStepFlag = 0;
			}
			if (updateHeartrateFlag){
				updateHeartrateFlag = 0;
			}
			
		}
		
		return 0;
}
#endif

#if MAIN == TEST
volatile uint32_t changeModeFlag = 0;
volatile uint32_t refreshDefaultFlag = 0;
volatile uint32_t refreshCompassFlag = 0;
volatile uint32_t refreshExerciseFlag = 0;

int main(void){
		DisableInterrupts();
    PLL_Init(Bus80MHz); // PLL Init
		//UART_Init(); // UART Input Init (Baud rate : 115200)
		Unified_Port_Init(); // All of *our* Port + Component Initializations
		Unified_Timer_Init();	// 
		display_init();				// Main display initialization (Reflect & rotate)
		start_sequence();
		DelayWait10ms(400);
		clear_screen();
		default_mode_init();
		EnableInterrupts();
	while(1){
		WaitForInterrupt();
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
	return 0;
}
#endif