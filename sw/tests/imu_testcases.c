// Author : Ethan Syed
// Testcases for
// Inertial Measurement Unit
// PN : LSM303A

#include "stdint.h"
#include "../inc/tm4c123gh6pm.h" // TM4C register definitions
#include "../inc/CortexM.h" // Clock delay & interrupt control
#include "../inc//PLL.h"

#include "../src/globalVars.h"
#include "../inc/UART.h"
#include "../src/LSM303A_driver.h"

#include "../src/LSM303AGR_Accelerometer.h"

void get_magnetometer_raw();
void get_accelerometer_raw();

void test_magnetometer(uint32_t raw);
void test_accelerometer(uint32_t raw);

// Function definitions
void DelayWait10ms(uint32_t n);

#if MAIN == LSM303AGR
int main(void){
	DisableInterrupts();
	PLL_Init(Bus80MHz); // PLL Init
	UART_Init();
	LSM303A_Init();
	UART_OutString("Connected");
	OutCRLF();
	EnableInterrupts();
	
	DelayWait10ms(100);
	while(1){
		OutCRLF();
		UART_OutString("X axis: ");
		UART_OutInt16(get_x_axis());
//		UART_OutString(" Y axis: ");
//		UART_OutInt16(get_y_axis());
//		UART_OutString(" Z axis: ");
//		UART_OutInt16(get_z_axis());
		//poll_status_reg();
		OutCRLF();
		DelayWait10ms(100);
	}
	return 0;
}
#endif

