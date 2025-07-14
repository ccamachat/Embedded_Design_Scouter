// Authors : Ethan S
// Inertial Measurement Unit (IMU)
// Accelerometer + Magnetometer

#include <stdint.h>
#include "../inc/I2C2.h"
#include "lsm303agr.h"
#include "inc/tm4c123gh6pm.h"

#include "LSM303AGR_Accelerometer.h" //Accelerometer Driver
#include "LSM303AGR_Magnetometer.h" // Magnetometer Driver

// Interfaced using I2C2
// Written with example from Valvanoware/EdgeInterrupt_4C123
/*	
-------- Port Mapping --------
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

#define I2C2_Frequency 100000 // 100kHz
#define busFreq 80000000 // 80Mhz

/* Function Definitions */


// Interface using I2C2
void LSM303A_Init(void){
	I2C2_Init(I2C2_Frequency, busFreq);
	
	initialize_accelerometer_interrupts(); // tm4c int pin initializations
	init_accelerometer();									 // LSM303AGR reg initializations
	
	//initialize_magnetometer_interrupts(); // tm4c int pin initializations

}

int recieve_accelerometer_raw(void){
	get_x_axis();
	return 0;
}	

int recieve_magnetometer_raw(void){
	return 0;
}

int xyz_heading(){
	return 0;
}
