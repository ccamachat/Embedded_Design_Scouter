// Author : Ethan Syed
// Inertial Measurement Unit (IMU)
// Accelerometer
// Adafruit BoB

#include <stdint.h>
#include "../inc/I2C2.h"
#include "lsm303agr.h"
#include "inc/tm4c123gh6pm.h"

#include "../inc/UART.h"

// function definitions
int16_t convert(uint8_t high, uint8_t low);


// state definitions
uint32_t highres = 0;

/* Notes:
High res mode CTRL_REG4_A[3 and low power mode CTRL_REG1_A[3]
	cannot be on simultaneously
	
You can transition between modes while system is running 
	Must wait a minimum time of 1 / ODR (Output data rate) seconds

*/

void initialize_accelerometer_interrupts(void){
	// PC5 & PB6
	SYSCTL_RCGCGPIO_R     |= 0x02;            // activate port B
  while((SYSCTL_PRGPIO_R & 0x02) == 0){}; 	// wait
		
	SYSCTL_RCGCGPIO_R     |=  0x04;         // Activate clock for Port C
  while((SYSCTL_PRGPIO_R & 0x04) != 0x04){};  // Allow time for clock to start

	// -------- Initialize PB6 + PC5 -------- //
	// PB6
	//GPIO_PORTB_PCTL = // Not needed 
	GPIO_PORTB_CR_R = 0x40;	// allow changes to PB6
	GPIO_PORTB_DIR_R &= ~0x40;           // make PB6 input
	GPIO_PORTB_AFSEL_R    &= ~0x40;            // Disable alt funct on PB6
	GPIO_PORTB_DEN_R      |= 0x40;            // enable digital I/O on PB6
	GPIO_PORTB_AMSEL_R    &= ~0x40;           // disable analog functionality on PB6
	
	GPIO_PORTB_IS_R &= ~0x40;     // PB6 is edge-sensitive
	GPIO_PORTB_IBE_R &= ~0x40;    //     PB6 is not both edges 
	GPIO_PORTB_IEV_R |= 0x40;    //     PB6 rising edge event
	
	GPIO_PORTB_ICR_R = 0x40;      // clear flag6
  GPIO_PORTB_IM_R |= 0x40;      // arm interrupt on PB6
	
	NVIC_PRI0_R = (NVIC_PRI0_R&0xFFFF00FF)|0x0000E000; // priority 7 
  NVIC_EN0_R = 0x00000002;      // (h) enable IRQ 1 in NVIC
	
	// PC5
	//GPIO_PORTC_PCTL = // Not needed 
	GPIO_PORTC_CR_R = 0x20;	// allow changes to PC5
	GPIO_PORTC_DIR_R &= ~0x20;           // make PC5 input
	GPIO_PORTC_AFSEL_R    &= ~0x20;            // disable alt funct on PC5
	GPIO_PORTC_DEN_R      |= 0x20;            // enable digital I/O on PC5
	GPIO_PORTC_AMSEL_R    &= ~0x20;           // disable analog functionality on PC5
	
	GPIO_PORTC_IS_R &= ~0x20;     // PC5 is edge-sensitive
	GPIO_PORTC_IBE_R &= ~0x20;    //     PC5 is not both edges 
	GPIO_PORTC_IEV_R |= 0x20;    //     PC5 rising edge event
	
	GPIO_PORTC_ICR_R = 0x20;      // clear flag5
  GPIO_PORTC_IM_R |= 0x20;      // arm interrupt on PC5
	
	NVIC_PRI0_R = (NVIC_PRI0_R&0xFF00FFFF)|0x00E00000; // priority 7 
  NVIC_EN0_R = 0x00000004;      // (h) enable IRQ 2 in NVIC
	
}

void init_accelerometer(void){
	uint8_t recv = 0;
//	I2C2_Send1(ACC_I2C_ADDRESS, 1);	// Tell Slave I will be reading from this address base
//	I2C2_Recv(ACC_I2C_ADDRESS, &recv, 1); // Listen for acknowledge
//	I2C2_Send1(LSM303AGR_CTRL_REG1_A, LSM303AGR_ODR_100_HZ + LSM303AGR_AXES_ENABLE); // Send subaddress + cmd
//	I2C2_Recv(LSM303AGR_CTRL_REG1_A, &recv, 1); // Listen for acknowledge
	uint8_t packet[2];
	
	packet[0] = LSM303AGR_CFG_REG_A_M;
	packet[1] = 0x00;
	int watch = I2C2_Send(ACC_I2C_ADDRESS, packet, 2);
	
	packet[0] = LSM303AGR_CFG_REG_C_M;
	packet[1] = 0x01;
	watch = I2C2_Send(ACC_I2C_ADDRESS, packet, 2);
	
	packet[0] = LSM303AGR_CTRL_REG1_A;
	packet[1] = LSM303AGR_ODR_100_HZ + LSM303AGR_AXES_ENABLE;
	watch = I2C2_Send(ACC_I2C_ADDRESS, packet, 2);
	
	//uint32_t watch = I2C2_Send3(ACC_I2C_ADDRESS, packet[0], packet[1], 0);
	//I2C_Write(ACC_I2C_ADDRESS, LSM303AGR_CTRL_REG1_A, packet[1]);
	
	
	
	packet[0] = LSM303AGR_CTRL_REG2_A;
	packet[1] = 0x00;
	I2C2_Send(ACC_I2C_ADDRESS, packet, 2);
	
	packet[0] = LSM303AGR_CTRL_REG3_A;
	packet[1] = 0x00;
	I2C2_Send(ACC_I2C_ADDRESS, packet, 2);
	
	packet[0] = LSM303AGR_CTRL_REG4_A;
	packet[1] = 0x00;
	I2C2_Send(ACC_I2C_ADDRESS, packet, 2);
	

	
	
	
			// Set output data rate 100Hz + Enable XYZ axes
	I2C2_Send1(LSM303AGR_CTRL_REG1_A, LSM303AGR_ODR_100_HZ + LSM303AGR_AXES_ENABLE); 

		// Block Data Continuous mode + Full Scale Selection 2g + High-res mode
	I2C2_Send1(ACC_I2C_ADDRESS, 1);
	
	I2C2_Send1(LSM303AGR_CTRL_REG4_A, LSM303AGR_BlockUpdate_Continous + LSM303AGR_FULLSCALE_2G
		+ LSM303AGR_HR_ENABLE); 
	highres = 1;
	
		// Disable FIFO mode
	I2C2_Send1(LSM303AGR_FIFO_CTRL_REG_A, 0x00); 
	
		// Enable IA1 + IA2 (Interrupt on data ready IA2)
	I2C2_Send1(LSM303AGR_CTRL_REG3_A, LSM303AGR_IT1_AOI1 + LSM303AGR_IT1_AOI2 + LSM303AGR_IT1_DRY2); 
	
}

// Protocol requirements
// Send start signal (high to low on data line while SCL line held high)

void disable_ODR(void){
		I2C2_Send1(LSM303AGR_CTRL_REG1_A, 0x00 + LSM303AGR_AXES_DISABLE); 
}

int16_t get_x_axis(void){
	int16_t final = 0;
	// 12 - bits of data
	if(highres){
		uint8_t high_data = 0; 
		uint8_t low_data = 0;
		
		uint8_t packet1[2];
		packet1[0] = LSM303AGR_OUT_X_H_A;
		I2C2_Recv(ACC_I2C_ADDRESS, packet1, 2);
		
		uint8_t packet2[1];
		I2C2_Recv(LSM303AGR_OUT_X_L_A, packet2, 1);
		
		final = convert(packet1[0], packet2[0]);
	}
	return final;
}

int16_t get_y_axis(void){
		int16_t final = 0;
	// 12 - bits of data
	if(highres){
		uint8_t high_data = 0; 
		uint8_t low_data = 0;
		I2C2_Recv(LSM303AGR_OUT_Y_H_A, &high_data, 1);
		I2C2_Recv(LSM303AGR_OUT_Y_L_A, &low_data, 1);
		
		final = convert(high_data, low_data);
	}
	return final;
}

int16_t get_z_axis(void){
	int16_t final = 0;
	// 12 - bits of data
	if(highres){
		uint8_t high_data = 0; 
		uint8_t low_data = 0;
		I2C2_Recv(LSM303AGR_OUT_Z_H_A, &high_data, 1);
		I2C2_Recv(LSM303AGR_OUT_Z_L_A, &low_data, 1);
		
		final = convert(high_data, low_data);
	}
	return final;
}

int16_t convert(uint8_t high,uint8_t low){
	    // Combine the high and low bytes into a 12-bit signed integer
    int16_t result = (high << 4) | (low >> 4);
    // Extend sign if necessary
    if (result & 0x0800) {
        result |= 0xF000;
    }
    return result;
}
	
void poll_status_reg(void){
	uint8_t signal = 0;
	I2C2_Send1(ACC_I2C_ADDRESS, 1); 
	I2C2_Recv(ACC_I2C_ADDRESS, &signal, 1);
	
	I2C2_Recv(LSM303AGR_STATUS_REG_A, &signal, 1);
	
}

