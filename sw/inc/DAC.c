/**
 * @file DAC.c
 * @author your name (you@domain.com)
 * @brief Low level driver for the DAC. Documentation comments are specifically
 *        for the TLV5618 12-bit SPI DAC. Modify for your particular DAC.
 * @version 0.3.0
 * @date 2023-02-14
 *
 * @copyright Copyright (c) 2023
 * @note Reference datasheet:
 *      https://www.ti.com/lit/ds/symlink/tlv5618a.pdf?ts=1676400608127&ref_url=https%253A%252F%252Fwww.google.com%252F
 */
#include <stdio.h>
#include <stdint.h>
#include "./DAC.h"
#include "./tm4c123gh6pm.h"
#include "./inc//Dump.h"

int dac_init() {
    /**
     * Unified_Port_Init in Lab5.c calls Port_D_Init, which initializes the Port
     * D GPIOs for the appropriate alternate functionality (SSI).
     *
     * According to Table 15-1. SSI Signals in the datasheet, this corresponds
     * to SSI1. The corresponding Valvanoware register defines are at L302 and
     * L2670 in inc/tm4c123gh6pm.h. Use this in combination with the datasheet
     * or any existing code to write your driver! An example of how to
     * initialize SSI is found in L741 in inc/ST7735.c.
     */

	//TODO: Modify the initialization to Port D
	//GPIO_PORTA_AFSEL_R |= 0x2C;           // enable alt funct on PA2,3,5
  //GPIO_PORTA_DEN_R |= 0x2C;             // enable digital I/O on PA2,3,5
                                        // configure PA2,3,5 as SSI
  //GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFF0F00FF)+0x00202200;
  //GPIO_PORTA_AMSEL_R &= ~0x2C;          // disable analog functionality on PA2,3,5
//  SSI1_CR1_R &= ~SSI_CR1_SSE;           // disable SSI
//  SSI1_CR1_R &= ~SSI_CR1_MS;            // master mode
//                                        // configure for system clock/PLL baud clock source
//  SSI1_CC_R = (SSI1_CC_R&~SSI_CC_CS_M)+SSI_CC_CS_SYSPLL;
////                                        // clock divider for 3.125 MHz SSIClk (50 MHz PIOSC/16)
////  SSI0_CPSR_R = (SSI0_CPSR_R&~SSI_CPSR_CPSDVSR_M)+16;
//                                        // clock divider for 8 MHz SSIClk (80 MHz PLL/24)
//                                        // SysClk/(CPSDVSR*(1+SCR))
//                                        // 80/(10*(1+0)) = 8 MHz (slower than 4 MHz)
//  SSI1_CPSR_R = (SSI1_CPSR_R&~SSI_CPSR_CPSDVSR_M)+10; // must be even number
//  SSI1_CR0_R &= ~(SSI_CR0_SCR_M |       // SCR = 0 (8 Mbps data rate)
//                  SSI_CR0_SPH |         // SPH = 0
//                  SSI_CR0_SPO);         // SPO = 0
//                                        // FRF = Freescale format
//  SSI1_CR0_R = (SSI1_CR0_R&~SSI_CR0_FRF_M)+SSI_CR0_FRF_MOTO;
//                                        // DSS = 8-bit data
//  SSI1_CR0_R = (SSI1_CR0_R&~SSI_CR0_DSS_M)+SSI_CR0_DSS_8;
//  SSI1_CR1_R |= SSI_CR1_SSE;            // enable SSI
	
	//new initialization
	
SYSCTL_RCGCSSI_R |= 0x02;  // Activate SSI1
volatile int delay = SYSCTL_RCGCSSI_R;


SSI1_CR1_R = 0;      // Disable SSI
SSI1_CPSR_R = 0x08; // Must be even number

SSI1_CR0_R &= ~(SSI_CR0_SCR_M | 0xf0);   // SCR = 0 (8 Mbps data rate)

// Clock divider for 8 MHz SSIClk (80 MHz PLL/24)
// SysClk/(CPSDVSR*(1+SCR))
// 80/(10*(1+0)) = 8 MHz (slower than 4 MHz)


                                  // FRF = Freescale format
SSI1_CR0_R |= (SSI_CR0_SPO | SSI_CR0_DSS_16); // spo = 1, 16 bit data
SSI1_CR1_R |= SSI_CR1_SSE;        // Enable SSI
DumpInit();
JitterInit();
		return 1; 
	

}

//maybe cut r9 and r10

int dac_output(uint16_t data) {
    // An example of how to send data via SSI is found in L534 of inc/ST7735.c.
    // Remember that 4 out of the 16 bits is for DAC operation. The last 12 bits
    // are for data. Read the datasheet! 
//    while((SSI1_SR_R&SSI_SR_BSY)==SSI_SR_BSY){};
//		//TFT_CS = TFT_CS_LOW;
//		//DC = DC_COMMAND;
//		SSI1_DR_R = data;                        // data out
//                                        // wait until SSI0 not busy/transmit FIFO empty
//		while((SSI1_SR_R&SSI_SR_BSY)==SSI_SR_BSY){};
//			
			while((SSI1_SR_R&0x00000002)==0){};// wait until room in FIFO
		//SSI1_DR_R = data & 0x0fff; // data out
				//JitterMeasure();
			//printf("%d\r\n", JitterGet());
	SSI1_DR_R = 500;
		return 1; // UNIMPLEMENTED
}
