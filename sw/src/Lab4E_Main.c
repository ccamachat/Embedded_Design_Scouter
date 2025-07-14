// -------------------------------------------------------------------
// File name:     Lab4E_Main.c
// Description:   This code is the main loop for the new MQTT Clock Control IOT Lab
//               
// Authors:       Mark McDermott
// Date:          June 6, 2023
//
// 

#include <stdio.h>
#include <stdint.h>

#include "inc/tm4c123gh6pm.h"
#include "inc/ST7735.h"
#include "inc/PLL.h"
#include "inc/Timer2A.h"
#include "inc/Timer5A.h"
#include "inc/UART.h"
#include "inc/UART5.h"
#include "inc/esp8266_base.h"
#include "inc/MQTT.h"
#include "globalVariables.h"
#include "Lab3_4.h"
//#include "Lab4E.h"

/** File includes. */
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "ADC.h"
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
#include "../inc/Timer0A.h"
#include "../src/time.h"

uint32_t         Mode_Value;      //
uint32_t         Left_Value;      //
uint32_t         Right_Value;     //
uint32_t         Up_Value;        //
uint32_t         Down_Value;      //


char b2w_cmd_g[24]; //this is the global variable we should update with info we want to send to MQTT

//----- Prototypes of functions in startup.s  ----------------------
//
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // Go into low power mode

void pause(void){
	while((PF0==0x01)&&(PF4==0x10)){
		for(int32_t i=0; i<0x80000; i++){
		}
	}
	while(!((PF0==0x01)&&(PF4==0x10))){
		for(int32_t i=0; i<0x80000; i++){
		}
	}
}
/** Function declarations. */
/**
 * @brief DelayWait10ms delays the current process by n*10ms. Approximate.
 * 
 * @param n Number of times to delay 10ms.
 * @note Based on a 80MHz clock.
 */
void DelayWait10ms(uint32_t n);

/**
 * @brief Blocks the current process until PF4 (Left Button <=> SW1) is pressed.
 */
void Pause(void);

/** Entry point. */

void Interrupt_Seconds(void);
void Timer_CountDown(void);
void CheckAlarm(void);
// -----------------------------------------------------------------
// -------------------- MAIN LOOP ----------------------------------
//
int Lab4E_main(void){  
  DisableInterrupts();            // Disable interrupts until finished with inits     
  PLL_Init(Bus80MHz);             // Bus clock at 80 MHz
  UART_Init();                    // Allow us to talk to the PC via PuTTy!
  UART5_Init();                   // Enable ESP8266 Serial Port
  ST7735_InitR(INITR_REDTAB);     // Start up display.
  Valvano_Unified_Port_Init();						// Initialize the Ports used for this lab
	
	ST7735_OutString("Reseting ESP\n");
  Reset_8266();                   // Reset the WiFi module
  SetupWiFi();                    // Setup communications to MQTT Broker via 8266 WiFi
	
  Timer2A_Init(&MQTT_to_TM4C, 400000, 7);         // Check/Get data from the ESP every 5ms 
  Timer5A_Init(&TM4C_to_MQTT, 80000000, 7);       // Send data back to MQTT Web App every second 
  //Timer5A_Init(&TM4C_to_MQTT, 40000000, 7);

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
	Timer0A_Init(&Interrupt_Seconds, 80000000, 1); 
		//Initialize Lookup Table for clock hand end point values!
		Initialize_Lookup_Table();
				/* Allows any enabled timers to begin running. */
			Lab3_main(); //redoes the initialization
  EnableInterrupts();
	
	//ST7735_SetCursor(0,10);
	//ST7735_OutString("Press SW to toggle mode:");
	//main2();
	
	 /* Print starting message to the PC and the ST7735. */
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_SetCursor(0, 0);
    ST7735_OutString(
        "ECE445L Lab 3 & 4.\n"
        "Press SW1 to start.\n");
    UART_OutString(
        "ECE445L Lab 3 & 4.\r\n"
        "Press SW1 to start.\r\n");
    //Pause();
		DelayWait10ms(3);
		//Lab3_main(); //redoes the initialization
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
		Pause();
	
  //Integrate your lab 3 here
  while(1){ 
		ST7735_SetCursor(0,11);
		ST7735_OutString("Mode is now:    ");
		ST7735_SetCursor(13,11);
		ST7735_OutUDec( hourMode);
		hourMode ^= 0x01;
		pause();
		ST7735_SetCursor(5,13);
		ST7735_OutString(w2b_cmd);
		
//		UART5_OutString("mod,hou,min,sec\n");
////		unsigned char buf[20]; 
////		unsigned char* bufptr;
////		UART5_InString(bufptr, 20);
//		char out = UART5_InChar();
//		char* outptr = & out; 
//		ST7735_SetCursor(0,13);
//		//char* out = (char *) buf;
//		ST7735_OutChar(out);

  }
	
	
}

