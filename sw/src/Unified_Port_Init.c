// ---------------------------------------------------------------------------
//
// File name:     Unified_Port_Init.c
// 
// Author:        Ethan Syed
// Orig gen date: April 17, 2024
// Last update:   April 17, 2024  
// 
// Description:   This is the unified Port initialization routine for 
//                Ethan Syed, Coty Camachat, Phebe Tan, and Neilan Garcia
//								final lab.
//								Consolidates definitions of GPIO handlers
//
// Usage:         Call Unified_Port_Init() if you want to initialize all ports (preferred)
//										As per our project defined pin mappings
//                Call the individual component inits as needed.
//								Avoid calling each component driver initialization in main
//
// ---------------------------------------------------------------------------

#include "../inc/tm4c123gh6pm.h"
#include "globalVars.h"

#include "LSM303A_driver.h"
#include "MAX30102_driver.h"
#include "ESP32-S-CAM_driver.h"
#include "../inc/ST7735.h"
#include "Buttons.h"

/* -------------------------------- Component Wiring --------------------------------
Ethan Display:

LED - 3.3V
SCK - PA2
SDA - PA5
A0 - PA6
RESET - PA7
CS - PA3
GND - Gnd
VCC - 3.3V


LSM303AGR:
Vin - 3.3V
3V - *Unconnected* / Can draw up to ~100mA if needed
GND - Gnd
SCL - PE4 (I2C2_SCL)
SDA - PE5 (I2C2_SDA)
INTM - PB7
CSMC - 3.3v
CSA - 3.3v
IA1 -	PC5
IA2 - PB6

*/




/* -------------------------------- Port Mapping --------------------------------

// ----------------    PORT A Initialization   --------------------------------
PA0 =
PA1 = 
PA2 = ST7735 (SSI0_CSLK)
PA3 = ST7735 (SSI0_FSS)
PA4 = ST7735 (SSI0_RX)
PA5 = ST7735 (SSI0_TX)
PA6 = ST7735 (GPIO)
PA7 = ST7735 (GPIO)


// ----------------    PORT B Initialization   --------------------------------


*/

void Unified_Component_Init(void){
	ST7735_InitR(INITR_REDTAB); // Display Init
	LSM303A_Init();							// Accelerometer + Magnetometer
	MAX30102_Init();						// Heartbeat + SpO2
	ESP32_Init();								// Internet + ESP32-S-CAM
}

void Unified_Port_Init(void){
	Unified_Component_Init();
	Button_Init();
}

/* -------------------------------- Port Handlers -------------------------------- */

// PB6 Step count (Accelerometer 1 / LSM303AGR)
// PB7 (Magnetometer LSM303AGR)
void GPIOPortB_Handler(void){
	if((GPIO_PORTB_RIS_R >> 6) & 0x1){
		//handle Accelerometer 1 int (Pedometer)
	}
	if((GPIO_PORTB_RIS_R >> 7) & 0x1){
		//handle Magnetometer
	}
	GPIO_PORTB_ICR_R = 0xC0;
}
// PC4 - Button 
// PC5 - Accelerometer 2 (LSM303AGR)
void GPIOPortC_Handler(void){
	if((GPIO_PORTC_RIS_R >> 4) & 0x1){
		//handle button press PC4
		SW2D = 0;
	}
	if((GPIO_PORTC_RIS_R >> 5) & 0x1){
		//handle Accelerometer 2 int
	}
	
	GPIO_PORTC_ICR_R = 0x30;
}
// PF4 - Button
void GPIOPortF_Handler(void){
	SW1D = 0;
	GPIO_PORTF_ICR_R = 0x10;
}
// PE2 - Button // NOT WORKING
void GPIOPortE_Handler(void){
	if(SW0D == 2){
		changeModeFlag = 1;
	}
	SW0D = 0;
	GPIO_PORTE_ICR_R = 0x4;
}