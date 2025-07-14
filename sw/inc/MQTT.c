// ----------------------------------------------------------------------------
//
// File name: MQTT.c
//
// Description: This code is used to bridge the TM4C123 board and the MQTT Web Application
//              via the ESP8266 WiFi board

// Authors:       Mark McDermott
// Orig gen date: June 3, 2023
// Last update:   July 21, 2023
//
// ----------------------------------------------------------------------------


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "Clock.h"
#include "PLL.h"

#include "UART.h"
#include "UART5.h"
#include "esp8266.h"
#include "src/globalVariables.h"
#include "src/Deprecated_Display.h"

#define DEBUG1                // First level of Debug
#define DEBUG2                // Second level of Debug
#define DEBUG3                // Third level of Debug
//#undef  DEBUG1                // Comment out to enable Debug1
//#undef  DEBUG2                // Comment out to enable Debug2
//#undef  DEBUG3                // Comment out to enable Debug3

#define UART5_FR_TXFF            0x00000020  // UART Transmit FIFO Full
#define UART5_FR_RXFE            0x00000010  // UART Receive FIFO Empty
#define UART5_LCRH_WLEN_8        0x00000060  // 8 bit word length
#define UART5_LCRH_FEN           0x00000010  // UART Enable FIFOs
#define UART5_CTL_UARTEN         0x00000001  // UART Enable

// ----   Function Prototypes not defined in header files  ------
// 
void EnableInterrupts(void);    // Defined in startup.s

// ----------   VARIABLES  ----------------------------
//
extern uint32_t         Mode_Value;      //
extern uint32_t         Left_Value;      //
extern uint32_t         Right_Value;     //
extern uint32_t         Up_Value;        //
extern uint32_t         Down_Value;      //

extern uint32_t         ADCvalue;

char                    input_char;
char                    serial_buf[128];

char                    w2b_cmd[2];                   // MQTT Web App Command

static uint32_t         bufpos          = 0;
static uint32_t         parse           = 0;

extern unsigned short   Hour;
extern unsigned short   Minute;
extern unsigned short   Second;

uint32_t                HourLast        = 0;
uint32_t                MinuteLast      = 0;
uint32_t                SecondLast      = 0;
uint32_t                ADCvalueLast    = 0;

uint16_t modeOut, hourOut, minOut, secOut;

// --------------------------     W2B Parser      ------------------------------
//
// This parser decodes and executes commands from the MQTT Web Appication 
//
void Parser(void) {
  
  uint8_t cmd_num = atoi(w2b_cmd);     // Need to convert ASCII command to integer
  
  unsigned short New_Hour;
  unsigned short New_Minute;
  unsigned short New_Second;
  
  // ----  Command #1: Toggle MODE select 12 or 24 hour
  //
  if(w2b_cmd[0] == 'T')  { //T, this should be ok
      if (hourMode == 0x1) {
				Twentyto12();
				hourMode =0x0;
				
				//convert 24 hour to 12 hour
				hourOut = 0;
				minOut = 0;
				secOut = 0;
			}
      else if (hourMode == 0x0){
				Twelveto24();
				hourMode = 0x1;
				//convert 12 hour to 24 hour
				hourOut = 2;
				minOut = 3;
				secOut = 4;
			}
  }
	else if (w2b_cmd[0] == 'K'){
			setFlag = 0;
			if (currMode == 1) alarmFlag = 1;
			if (currMode == 2) timerFlag = 1;
	}
	else if (w2b_cmd[0] == 'Z'){
			soundOn = 0; //turn of PWM module using global var
			if(alarmFlag){
				alarmFlag = 0;
			}
			if (timerFlag){
				
			timerFlag = 0;
			}
	}
  else if(w2b_cmd[0] == 'c'){ //c
		ST7735_FillScreen(ST7735_BLACK);
		currMode = 0; 
		setFlag = 1;
		//switch to clock display and enter set mode. 
		//once its pressed again, set the web value to the board clock
		//set clock value
		//new_hours = 0;
//		new_hours = /* set new hour value */;
//    new_minutes = /* set new minute value */;
//    new_seconds = /* set new second value */;
		
	}
	else if (w2b_cmd[0] == 'a'){ //a
		ST7735_FillScreen(ST7735_BLACK);
		currMode = 1;
		setFlag = 1;
		//set alarm value
//		alarm_hours1 = /* set alarm hour tens place */;
//    alarm_hours2 = /* set alarm hour units place */;
//    alarm_minutes1 = /* set alarm minute tens place */;
//    alarm_minutes2 = /* set alarm minute units place */;
	}
	else if (w2b_cmd[0] == 't'){ //t
		ST7735_FillScreen(ST7735_BLACK);
		currMode = 2;
		setFlag = 1;
		//set timer value
//		timer_hours1 = /* set timer hour tens place */;
//    timer_hours2 = /* set timer hour units place */;
//    timer_minutes1 = /* set timer minute tens place */;
//    timer_minutes2 = /* set timer minute units place */;
//    timer_seconds1 = /* set timer second tens place */;
//    timer_seconds2 = /* set timer second units place */;
		
	}
	else if (w2b_cmd[0] == 'h'){ //h
		Decrement_Hours_Web();

	}
	else if (w2b_cmd[0] == 'm'){ //m
		Decrement_Minutes_Web();

		//decrement  minutes
	}
	else if (w2b_cmd[0] == 's'){ //s
		Decrement_Seconds_Web();
		//decrement seconds
	}
	else if (w2b_cmd[0] == 'H'){ //H
		Increment_Hours_Web();

	}
	else if (w2b_cmd[0] == 'M'){ //M
		Increment_Minutes_Web();
		//increment Minutes
	}
	else if (w2b_cmd[0] == 'S'){ //S
		Increment_Seconds_Web();
		//increment Seconds
	}
		  
}
  
// -----------------------  TM4C_to_MQTT Web App -----------------------------
// This routine publishes clock data to the
// MQTT Web Application via the MQTT Broker
// The data is sent using CSV format:
// MODE, Hour, Minute, Second, <- Need dangling comma to terminate string in ESP8266
// 0/1,  0-23,  0-59,    0-59
//
// ----------------------------------------------------------------------------
//    
//    Convert this routine to use a FIFO
//
// 
void TM4C_to_MQTT(void){
  char b2w_cmd[24]  = ""; 
  //TODO: Add other commands here
	//sprintf(b2w_cmd, "%02d,%02d,%02d,%02d\n", Mode_Value, new_hours, new_minutes, new_seconds); 
	if (currMode == 0x0){ //if the current mode is CLOCK: send current time 
		sprintf(b2w_cmd, "%02d,%02d,%02d,%02d,%02d\n", Mode_Value, new_hours, new_minutes, new_seconds, currMode);  // Msg is comma seperated, and must end with '\n'          
	}
	else if (currMode == 0x1){ //if the current mode is ALARM: send the time the alarm is set to with seconds replaced with 0
		sprintf(b2w_cmd, "%02d,%02d,%02d,00,%02d\n", Mode_Value, alarm_hours1*10 + alarm_hours2, alarm_minutes1*10 + alarm_minutes2, currMode);  // Msg is comma seperated, and must end with '\n'          
	}
	else { //if the current mode is TIMER: send the timer set value and the decrements until it reaches 0
		sprintf(b2w_cmd, "%02d,%02d,%02d,%02d,%02d\n", Mode_Value, timer_hours1*10+timer_hours2, timer_minutes1*10+timer_minutes2, timer_seconds1*10 + timer_seconds2, currMode);
	}
  UART5_OutString(b2w_cmd);      

  #ifdef DEBUG1
   UART_OutString("B2W: ");
   UART_OutString(b2w_cmd);         
   UART_OutString("\r\n"); 
  #endif 
}
 
// -------------------------   MQTT_to_TM4C  -----------------------------------
// This routine receives the command data from the MQTT Web App and parses the
// data and feeds the commands to the TM4C.
// -----------------------------------------------------------------------------
//
//    Convert this routine to use a FIFO
//
// 
void MQTT_to_TM4C(void) {
  parse = 0x0;
    
  if ((UART5_FR_R & UART5_FR_RXFE) ==0 ) {  // Check to see if a there is data in the RXD buffer
    input_char =(UART5_DR_R & 0xFF);        // Read the data from the UART
        
      if (input_char != '\n')               // If the incoming character is not a newline then process byte
      {                                     
        serial_buf[bufpos] = input_char;    // The buffer position in the array get assigned to the current read
        bufpos++;                           // Once that has happend the buffer advances,doing this over and over
                                            // again until the end of package marker is read.
        //UART2_OutChar(input_char);        // Debug only
      }
      else if (input_char == '\n')
      {
        serial_buf[bufpos] = ',';           // Add a comma for strcpy below
        bufpos++;
                
        if (bufpos  > 0) {
          strcpy(w2b_cmd, strtok(serial_buf, ","));
          parse = 0x1;                      // Parse incoming data
        }
        
        bufpos = 0;                         // Reset for next string
        
        #ifdef DEBUG1
          UART_OutString("W2B: ");
          UART_OutString(w2b_cmd);
          UART_OutString("\r\n");
        #endif
      } 
      
    else if ((UART5_FR_R & UART5_FR_RXFE) !=0 ) {     // No new data in the RXD buffer -> Exit routine
    }  
  }
  if (parse == 0x1) Parser();                         // Call the parser if new Command received
  
}   // End of routine

