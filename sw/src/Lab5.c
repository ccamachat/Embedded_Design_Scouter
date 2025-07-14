/**
 * @file Lab5.c
 * @author Neilan Garcia (neilantgarcia@utexas.edu), Phebe Tan(phebe.tan@austin.utexas.edu), Jonathan Valvano, Matthew Yu
 *    <Aeybel Varghese, 16990>
 * @brief
 *    An empty main file for running lab5.
 *    Feel free to edit this to match your specifications.
 *
 *    For this lab, the team must implement an audio player. The audio player
 *    must interface with (at minimum) two switches and the TLV5618 SPI DAC. The
 *    audio player must be able to play, pause, and rewind music, and should
 *    have extra functionality defined by the user in the requirements document.
 *    The LCD and other hardware may be used.
 * 
 *    Starter code includes the function definitions for the TLV5618 SPI
 *    DAC. The team must implement this driver and the driver must be written at
 *    a low level without calling TivaWare or ValvanoWare driver code. See
 *    inc/DAC.h for more details.
 * 
 *    The TA (Matthew) suggests the students consider the data structure design
 *    and abstractions required to play songs. What is a song? What is a note?
 *    What is a rest? How can one play chords? How can one queue songs?
 * 
 *    Other than the TLV5618 driver, this assignment is open ended, so teams
 *    must architect features of this audio player beyond the base requirements
 *    and design the relevant drivers. Feel free to use drivers made from
 *    previous labs (or build off of Lab 3 and 4). 
 * 
 *    Good luck!
 * 
 * @version 0.1
 * @date 2024-02-22 <REPLACE WITH DATE OF LAST REVISION>
 *
 * @copyright Copyright (c) 2022
 * @note Potential Pinouts:
 *    Backlight (pin 10) connected to +3.3 V
 *    MISO (pin 9) unconnected
 *    SCK (pin 8) connected to PA2 (SSI0Clk)
 *    MOSI (pin 7) connected to PA5 (SSI0Tx)
 *    TFT_CS (pin 6) connected to PA3 (SSI0Fss)
 *    CARD_CS (pin 5) unconnected
 *    Data/Command (pin 4) connected to PA6 (GPIO)
 *    RESET (pin 3) connected to PA7 (GPIO)
 *    VCC (pin 2) connected to +3.3 V
 *    Gnd (pin 1) connected to ground
 * 
 *    
 *
 *    Warning. Initial code for the RGB driver creates bright flashing lights. Remove this code
 *    and do not run if you have epilepsy.
 */

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2021

 Copyright 2022 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

/** File includes. */
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "Switches.h"
#include "../inc/Timer0A.h"
#include "../inc/Timer1A.h"
#include "Sound.h"

/* Register definitions. */
#include "../inc/tm4c123gh6pm.h"
/* Clocking. */
#include "../inc/PLL.h"
/* Clock delay and interrupt control. */
#include "../inc/CortexM.h"
/* Initialization of all the pins. */
#include "../inc/Valvano_Unified_Port_Init.h"
/* Talking to PC via UART. */
#include "../inc/UART.h"
/* ST7735 display. */
#include "../inc/ST7735.h"
/* Add whatever else you need here! */
#include "../lib/RGB/RGB.h"

#include "./Unified_Timer_Init.h"

/* NOTE: We suggest using the ./inc/ADCSWTrigger.h and the ./inc/TimerXA.h headers. */
#include "../inc/DAC.h"
#include "../inc/Dump.h"


/** MMAP Pin definitions. */
#define PF0   (*((volatile uint32_t *)0x40025004)) // Left Button
#define PF1   (*((volatile uint32_t *)0x40025008)) // RED LED
#define PF2   (*((volatile uint32_t *)0x40025010)) // BLUE LED
#define PF3   (*((volatile uint32_t *)0x40025020)) // GREEN LED
#define PF4   (*((volatile uint32_t *)0x40025040)) // Right Button

//void Pause(void);
//void DelayWait10ms(uint32_t n);
void TimerInt(void);
void changeNote(void);
int notepntr = 0;

int pntr = 0;

int mainLab5(void) {
    DisableInterrupts();

    /* Interrupts currently being used:
       Timer0A, pri7 - RGB flashing
       UART0, pri7 - PC communication
    */
    
    /* PLL Init. */
    PLL_Init(Bus80MHz);

    /* Allow us to talk to the PC via PuTTy! Check device manager to see which
       COM serial port we are on. The baud rate is 115200 chars/s. */
    UART_Init();

    /* Start up display. */
    ST7735_InitR(INITR_REDTAB);

    /* Initialize all ports. */
    Valvano_Unified_Port_Init();
		
		//initailize dac
		dac_init();
    /* Start RGB flashing. WARNING! BRIGHT FLASHING COLORS. DO NOT RUN IF YOU HAVE EPILEPSY. */
    //RGBInit();
		//timer0A produces the sound
		Timer0A_Init(&TimerInt, C3, 2); 
		//timer 1A plays the actual note/changes the note
		//Timer1A_Init(&changeNote, 80000000,2);
    /* Allows any enabled timers to begin running. */
    EnableInterrupts();

    /* Print starting message to the PC and the ST7735. */
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_SetCursor(0, 0);
    ST7735_OutString(
        "ECE445L Lab 5.\n"
        "Press SW1 to start.\n");
    UART_OutString(
        "ECE445L Lab 5.\r\n"
        "Press SW1 to start.\r\n");
    //Pause();
		
    /* Stop RGB and turn off any on LEDs. */
    //RGBStop();
    PF1 = 0;
    PF2 = 0;
    PF3 = 0;

    /* Reset screen. */
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_SetCursor(0, 0);
    ST7735_OutString("Starting...\n");
    UART_OutString("Starting...\r\n");

    while (1) {
        /* TODO: Write your code here! */
      WaitForInterrupt();
			//PlaySound(pntr);
				ST7735_SetCursor(2,2);
				ST7735_OutUDec(pntr);
			
    }
    return 1;
}

/** Function Implementations. */
//void DelayWait10ms(uint32_t n) {
//    uint32_t volatile time;
//    while (n){
//        time = 727240 * 2 / 91;  // 10msec
//        while (time){
//            --time;
//        }
//        --n;
//    }
//}

//void Pause(void) {
//    while (PF4 == 0x00) {
//        DelayWait10ms(10);
//    }
//    while (PF4 == 0x10) {
//        DelayWait10ms(10);
//    }
//}

void TimerInt(void){
	PlaySound(pntr);
	pntr = (pntr + 1)  & (0x3F);

}
void changeNote(){
	int NotePitch = songs[driftveil].pitches[songs[driftveil].notes[notepntr].idx];
	int NoteLength = songs[driftveil].notes[notepntr].length;
	notepntr = (notepntr +1);
	
	//changes Timer0 reload value 
	TIMER0_CTL_R &= ~0x00000001;     // 1) disable timer0A during setup
  TIMER0_CFG_R = 0x00000000;       // 2) configure for 32-bit timer mode
  TIMER0_TAMR_R = 0x00000002;      // 3) configure for periodic mode, default down-count settings
  TIMER0_TAILR_R = NotePitch-1;       // 4) reload value
  TIMER0_TAPR_R = 0;               // 5) 12.5ns timer0A
  TIMER0_ICR_R = 0x00000001;       // 6) clear timer0A timeout flag
  
	
	//change Timer1 reload value.
	TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER1_TAILR_R = NoteLength-1;    // 4) reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
  TIMER1_ICR_R = 0x00000001;    // 6) clear TIMER1A timeout flag
  TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A	
	TIMER0_CTL_R |= 0x00000001;      // 10) enable timer0A
}
