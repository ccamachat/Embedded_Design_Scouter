/**
 * @file Lab3_4.c
 * @author Neilan Garcia (neilantgarcia@utexas.edu), Phebe Tan(phebe.tan@austin.utexas.edu), Jonathan Valvano, Matthew Yu (matthewjkyu@gmail.com)
 *    <Aeybel Varghese, 16990>
 * @brief
 *    A default main file for running lab3 and 4.
 *    Feel free to edit this to match your specifications.
 *
 *    For these two labs, the student must implement an alarm clock with various 
 *    functions (lab 3) and then integrate it with a remote server, Blynk (lab 4). 
 *    This assignment is open ended, so students must plan the features of their 
 *    alarm clock (besides some base required features), design drivers for peripherals 
 *    used by the clock (ST7735 drawing routines, switch debounce drivers, and so forth), 
 *    and integrate it all together to have a functioning device. Good luck!
 * 
 * @version 0.2
 * @date 2022-09-12 <REPLACE WITH DATE OF LAST REVISION>
 *
 * @copyright Copyright (c) 2022
 * @note 
 *    We suggest following the pinouts provided by the 
 *    EE445L_Baseline_Schematic_Guide.pdf, found in the resources folder.
 *    Warning. Initial code for the RGB driver creates bright flashing lights. 
 *    Please remove this code and do not run if you have epilepsy.
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
#include "../src/ADC.h"
#include "globalVariables.h"
#include "Deprecated_Display.h"
#include "../inc/PWM.h"
#include "Switches.h"


/* Register definitions. */
#include "./inc/tm4c123gh6pm.h"
/* Clocking. */
#include "./inc/PLL.h"
/* Clock delay and interrupt control. */
#include "./inc/CortexM.h"
/* Initialization of all the pins. */
#include "./inc/Valvano_Unified_Port_Init.h"
/* Talking to PC via UART. */
#include "./inc/UART.h"
/* Talking to Blynk via the ESP8266. */
#include "./inc/Blynk.h"
/* ST7735 display. */
#include "./inc/ST7735.h"
/* Add whatever else you need here! */
#include "./lib/RGB/RGB.h"
#include "../src/Deprecated_Display.h"
#include "../inc/Timer0A.h"
#include "../src/time.h"

/* TODO: enable this for lab 4. */
#define LAB_4 false

/* TODO: We suggest using the ./inc/ADCSWTrigger.h and the ./inc/TimerXA.h headers. */

/** MMAP Pin definitions. */
#define PF0   (*((volatile uint32_t *)0x40025004)) // Left Button
#define PF1   (*((volatile uint32_t *)0x40025008)) // RED LED
#define PF2   (*((volatile uint32_t *)0x40025010)) // BLUE LED
#define PF3   (*((volatile uint32_t *)0x40025020)) // GREEN LED
#define PF4   (*((volatile uint32_t *)0x40025040)) // Right Button

	uint8_t new_hours, new_minutes, new_seconds;
	
	uint32_t butSelect, currMode, soundOn, lightMode, setFlag, alarmFlag, timerFlag, hourMode, AMFlag;
	uint8_t alarm_hours1,alarm_hours2, alarm_minutes1, alarm_minutes2;
	int8_t timer_hours1, timer_hours2, timer_minutes1, timer_minutes2, timer_seconds1, timer_seconds2;

/** Function declarations. */
/**
 * @brief DelayWait10ms delays the current process by n*10ms. Approximate.
 * 
 * @param n Number of times to delay 10ms.
 * @note Based on a 80MHz clock.
 */
//void DelayWait10ms(uint32_t n);

/**
 * @brief Blocks the current process until PF4 (Left Button <=> SW1) is pressed.
 */
void Pause(void);

/** Entry point. */

void Interrupt_Seconds(void);
void Timer_CountDown(void);
void CheckAlarm(void);
uint8_t new_hours, new_minutes, new_seconds;
int Lab3_main(void) {
    DisableInterrupts();

    /* Interrupts currently being used:
        Timer0A, pri7 - RGB flashing
        Timer2A, pri4 - ESP8266 sampling
        UART0, pri7 - PC communication
        UART5 (lab4), pri2 - ESP8266 communication
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
    
    /* Start RGB flashing. WARNING! BRIGHT FLASHING COLORS. DO NOT RUN IF YOU HAVE EPILEPSY. */
    RGBInit();

		/*ADC Initialize for potentiometer*/
		ADC_Init();
		
		// Initialize PWM at (almost) zero duty cycle (off)// 
		PWM0A_Init(0xFFFE, 0X2);
		
		// Initialize Port E for button usage 
		Button_Init();
		
		/* Initialize Timer0A for time based interrupts */
		new_hours = 12;
		new_minutes = 0;
		new_seconds = 0;
		currMode = 0; //INITIALIZE currMode to 1!!! will show alarm
		lightMode = 0;
		setFlag = 0; //we are not setting...
		timerFlag = 0, alarmFlag = 0;
		alarm_hours1 = 0;
		alarm_hours2 = 0;
		alarm_minutes1 = 0;
		alarm_minutes2 = 0;
		timer_hours1=0, timer_hours2=0, timer_minutes1=0, timer_minutes2=0, timer_seconds1=0, timer_seconds2 = 0;
		hourMode = 0;
		AMFlag = 0;
		Timer0A_Init(&Interrupt_Seconds, 80000000, 1); 
		//Initialize Lookup Table for clock hand end point values!
		Initialize_Lookup_Table();
				/* Allows any enabled timers to begin running. */
    EnableInterrupts();
		
		
		
    /* Print starting message to the PC and the ST7735. */
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_SetCursor(0, 0);
    ST7735_OutString(
        "ECE445L Lab 3 & 4.\n"
        "Press SW1 to start.\n");
    UART_OutString(
        "ECE445L Lab 3 & 4.\r\n"
        "Press SW1 to start.\r\n");
    Pause();
		
    /* Stop RGB and turn off any on LEDs. */
    RGBStop();
    PF1 = 0;
    PF2 = 0;
    PF3 = 0;
		
    /* Reset screen. */
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_SetCursor(0, 0);
    ST7735_OutString("Starting...\n");
    UART_OutString("Starting...\r\n");
		ST7735_FillScreen(ST7735_BLACK);
    while (1) {
			
			ST7735_SetCursor(2, 2);	
			
			uint32_t poten = ADC_In();
			butSelect = poten >> 10; //butSelect is the 4 segments of the slide potentiometer
			
			SelectedOption(butSelect); //the four segments sent to be converted to 3 menu options
			Select4Digit(butSelect);  //the four segments sent to be used at 4 digit selectors
			
			Select6Digit( (poten / 683) ); //should convert slide pot to nums between 0 and 5 for 6 digit select
			
			
      //WaitForInterrupt();		
			if(Display_Flag_Value() == 1){ //the flag has been set from the ISR so a second has passed by and the ISR is done
				
				Set_Display_Flag(0); //reset the flag

				Display_DigitalClock();
				if(setFlag == 0){
				Display_Menu();
				}
				//if setFlag is 1, then DONT DISPLAY MENU
				if(alarmFlag){

					CheckAlarm(); //constantly checking if alarm time is reached!
				}
				if(timerFlag){
					Timer_CountDown();
					
				}
				if(hourMode == 0x0){
				}
				else if (hourMode == 0x1){
				}
				if(currMode == 0){ //clock face
					if(setFlag == 1){
						//SelectedDigitClockMode
						DrawSelectedDigitClock(); //draws arrow (^) under numeric clock on analog display
//						new_hours = new_hours;
//						new_minutes	= new_minutes;
//						new_seconds --;
					}

					Display_ClockScreen(); //draws the bit map
					Increment_Clock_Display(); //adjusts the hands
					
				}
				else if(currMode == 1){ // alarm mode
					if(setFlag == 1){
								
								DrawSelectedDigitAlarm(); //displays the ^ under the digits
								//display a menuless screen
							}
					Display_AlarmScreen();
				}
				else if (currMode == 2){ // timer mode
					if(setFlag == 1){
								//SelectedDigitTimerMode
						DrawSelectedDigitTimer();
							}
					Display_TimerScreen();
					
					//WAIT FOR INPUT......
				}

				if (soundOn == 1){
					if (PWM0A_return_Duty() >= 0xFF){
						PWM0A_Duty(0x2);
					}
					else PWM0A_Duty(0x7FFF);
				}
				else PWM0A_Duty(0x2);
      }		
			ST7735_SetCursor(0,1);
			ST7735_OutUDec(butSelect);
		
	
			if (currMode == 0) { //on main clock screen print CLOCK
				ST7735_SetCursor(0,0);
				ST7735_OutString("CLOCK");
				
			}
			else if (currMode == 1){
				ST7735_SetCursor(0,0);
				ST7735_OutString("ALARM");
			}
			else if (currMode == 2) {
				ST7735_SetCursor(0,0);
				ST7735_OutString("TIMER");
			}
			//ST7735_OutUDec(currMode);		
			
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
void Timer_CountDown(void){
	if(timer_seconds1 == 0  && timer_seconds2 ==0 && timer_minutes1 == 0 && timer_minutes2 ==0 && timer_hours1 ==0 && timer_hours2 == 0){
	soundOn = 1;
	}
	else{
	timer_seconds2--; // Decrement units digit of seconds

    // If units digit of seconds is below zero, handle wrap-around
    if (timer_seconds2 < 0) {
        timer_seconds2 = 9; // Reset units digit of seconds to 9
        timer_seconds1--;   // Decrement tens digit of seconds

        // If tens digit of seconds is below zero, handle wrap-around
        if (timer_seconds1 < 0) {
            timer_seconds1 = 5;  // Reset tens digit of seconds to 5
            timer_minutes2--;    // Decrement units digit of minutes

            // If units digit of minutes is below zero, handle wrap-around
            if (timer_minutes2 < 0) {
                timer_minutes2 = 9;    // Reset units digit of minutes to 9
                timer_minutes1--;      // Decrement tens digit of minutes

                // If tens digit of minutes is below zero, handle wrap-around
                if (timer_minutes1 < 0) {
                    timer_minutes1 = 5;  // Reset tens digit of minutes to 5

                    // Decrement units digit of hours
                    timer_hours2--;

                    // If units digit of hours is below zero, handle wrap-around
                    if (timer_hours2 < 0) {
                        timer_hours2 = 9;  // Reset units digit of hours to 9
                        timer_hours1--;    // Decrement tens digit of hours

                        // If tens digit of hours is below zero, reset it to 1 (assuming it's a timer counting from 12:59:59 to 01:00:00)
                        if (timer_hours1 < 0) {
                            timer_hours1 = 1;
                        }
                    }
                }
            }
        }
    }
}
	}
void Interrupt_Seconds(void){
	if(setFlag && currMode ==0){
		//dont increment. 
	}
	else{
	new_seconds++;
	if (new_seconds == 60) {
        new_seconds = 0;   // Reset seconds to 0
        new_minutes++;   // Increment minutes

        // Check if minutes reached 60
        if (new_minutes == 60) {
            new_minutes = 0;   // Reset minutes to 0
            (new_hours)++;     // Increment hours

            // Check if hours reached 13
            if (new_hours == 13) { //THIS WILL NEVER GO TO 0
								if (AMFlag == 0){
									AMFlag =1;
								}
								else if (AMFlag == 1){
									AMFlag = 0;
								}
                new_hours = 1;   // Reset hours to 1
            }
        }
    }
	//Increment_Seconds(new_hours,new_minutes,new_seconds); //update the values in display!!!
	}
	Increment_Seconds(new_hours,new_minutes,new_seconds); //update the values in display!!!
}
//void Increment_alarm_hours1(void){
//	//alarm_hours1 = new_alarm_hours1;
//}
//void Increment_alarm_hours2(uint8_t new_alarm_hours2){
//	alarm_hours2 = new_alarm_hours2;
//}
//void Increment_alarm_minutes1(uint8_t new_alarm_minutes1){
//	alarm_minutes1 = new_alarm_minutes1;
//}
//void Increment_alarm_minutes2(uint8_t new_alarm_minutes2){
//	alarm_minutes2 = new_alarm_minutes2;
//}
void CheckAlarm(void){
	int alarmTime12Hour = alarm_hours1*10 +alarm_hours2;
	//new hours, new minutes, new seconds are in 12 hour mode. 
	if (hourMode ==1){ //if currently 24 hour mode. 
		alarmTime12Hour = alarm_hours1 * 10 + alarm_hours2;
    if (alarmTime12Hour > 12) { //need to convert it. 
        alarmTime12Hour -= 12;
    }
	}
	//Now incorporate AM/PM Flag in the alarm
	
	if (alarmTime12Hour == new_hours && alarm_minutes1 * 10 + alarm_minutes2 == new_minutes) {
        // Set soundOn flag to activate the alarm sound
        soundOn = 1;
    }


}
void Pause(void) {
    while (PF4 == 0x00) {
        //DelayWait10ms(10);
    }
    while (PF4 == 0x10) {
        //DelayWait10ms(10);
    }
}
