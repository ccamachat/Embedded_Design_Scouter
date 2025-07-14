// Author(s) : Ethan Syed
// Inertial Measurement Unit (IMU)
// Magnetometer
// Adafruit BoB

#include <stdint.h>
#include "../inc/I2C2.h"
#include "lsm303agr.h"
#include "inc/tm4c123gh6pm.h"

void initialize_magnetometer_interrupts(void){
	// PB7 
	// Port B should already be on; but reinitialized to decouple interrupt initializations
	SYSCTL_RCGCGPIO_R     |= 0x02;            // activate port B
  while((SYSCTL_PRGPIO_R & 0x02) == 0){}; 	// wait
		
	// PB7
	//GPIO_PORTB_PCTL = // Not needed 
	GPIO_PORTB_CR_R = 0x40;	// allow changes to PB7
	GPIO_PORTB_DIR_R &= ~0x40;           // make PB7 input
	GPIO_PORTB_AFSEL_R    &= ~0x40;            // Disable alt funct on PB7
	GPIO_PORTB_DEN_R      |= 0x40;            // enable digital I/O on PB7
	GPIO_PORTB_AMSEL_R    &= ~0x40;           // disable analog functionality on PB7
	
	GPIO_PORTB_IS_R &= ~0x40;     // PB7 is edge-sensitive
	GPIO_PORTB_IBE_R &= ~0x40;    //     PB7 is not both edges 
	GPIO_PORTB_IEV_R |= 0x40;    //     PB7 rising edge event
	
	GPIO_PORTB_ICR_R = 0x40;      // clear flag7
  GPIO_PORTB_IM_R |= 0x40;      // arm interrupt on PB7
	
	NVIC_PRI0_R = (NVIC_PRI0_R&0xFFFF00FF)|0x0000E000; // priority 7 
  NVIC_EN0_R = 0x00000002;      // (h) enable IRQ 1 in NVIC
}

void init_magnetometer(void){
	//LSM303AGR_CFG_REG_A_M = 0x0;	// Enable magnetometer (high resolution + cts mode)
	//LSM303AGR_CFG_REG_C_M = 0x1		// Enable magnetometer data-ready interrupt
}