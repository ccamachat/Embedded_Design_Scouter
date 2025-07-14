// Author : Ethan S.
// Hearbeat sensor

#include <stdint.h>

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

void MAX30102_Init(void);
void heartbeat_to_TM4C();

uint32_t get_raw(); // raw input
uint32_t get_heartbeat(int); //bpm