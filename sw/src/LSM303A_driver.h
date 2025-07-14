// Author : Ethan S
// Inertial Measurement Unit Sensor
#ifndef __LSM303A_driver__
#define __LSM303A_driver__
#include <stdint.h>

// Interfaced using I2C2
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

void LSM303A_Init(void);
void IMU_to_TM4C();
void TM4C_to_IMU();

int recieve_accelerometer_raw(void);
int recieve_magnetometer_raw(void);

// NOTES :
// Interrupt Initializations NEED testing!!!
	// Preferably apply an aribtrary high signal to relative pins
	// 		while in debugger mode and see if if stops in the GPIO handler
#endif