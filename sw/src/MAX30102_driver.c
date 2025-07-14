// Author : Ethan S.
// Hearbeat sensor

#include <stdint.h>
#include "../inc/I2C0.h"
#include "max30102.h"

// Interfaced using I2C0 
//
/*	
-------- Port Mapping --------
GND - Untied
RD - Untied
IRD - Untied
INT - Untied
Vin - 3.3V
SDA - PB3 (I2C0_SCL)
SCL - PB2 (I2C0_SDA)
GND - Gnd

*/
#define I2C0_Frequency 400000 //400kHz (Fastmode in Arduino code)
#define Bus_Frequency 80000000 //80MHz
void MAX30102_Init(void){
	I2C_Init_Freq(I2C0_Frequency, Bus_Frequency);
}

uint32_t get_raw(){
	return 0;
}