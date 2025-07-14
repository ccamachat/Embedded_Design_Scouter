// Author : Ethan S
// Port Mappings

#include "../inc/tm4c123gh6pm.h"
#include "../inc/CortexM.h"
#include "globalVars.h"
//	PE2 - Pri 5
//	PC4 - Mode Switching (Pri 7)
//	PF4 - Pri 5

void PortC_Button_Init(void){
	SYSCTL_RCGCGPIO_R     |=  0x04;         // Activate clock for Port C
  while((SYSCTL_PRGPIO_R & 0x04) != 0x04){};  // Allow time for clock to start
		
	GPIO_PORTC_CR_R = 0x10;	// allow changes to PC4
	GPIO_PORTC_DIR_R &= ~0x10;           // make PC4 input
	GPIO_PORTC_AFSEL_R    &= ~0x10;            // disable alt funct on PC4
	GPIO_PORTC_DEN_R      |= 0x10;            // enable digital I/O on PC4
	GPIO_PORTC_AMSEL_R    &= ~0x10;           // disable analog functionality on PC4
	
	GPIO_PORTC_IS_R &= ~0x10;     // PC4 is edge-sensitive
	GPIO_PORTC_IBE_R &= ~0x10;    //     PC4 is not both edges 
	GPIO_PORTC_IEV_R |= 0x10;    //     PC4 rising edge event
	
	GPIO_PORTC_ICR_R = 0x10;      // clear flag4
  GPIO_PORTC_IM_R |= 0x10;      // arm interrupt on PC4 
	
	NVIC_PRI0_R = (NVIC_PRI0_R&0xFF00FFFF)|0x00E00000; // priority 7 
  NVIC_EN0_R = 0x00000004;      // (h) enable IRQ 2 in NVIC
}
void PortF_Button_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x00000020; 	// activate clock for port F
  while((SYSCTL_PRGPIO_R & 0x00000020) == 0){}; //wait
  GPIO_PORTF_DIR_R &= ~0x10;    		// make PF4 in (built-in button)
  GPIO_PORTF_AFSEL_R &= ~0x10;  		// disable alt funct on PF4
  GPIO_PORTF_DEN_R |= 0x10;     		// enable digital I/O on PF4   
  GPIO_PORTF_PCTL_R &= ~0x000F0000; // configure PF4 as GPIO
  GPIO_PORTF_AMSEL_R = 0;       		// disable analog functionality on PF
  //GPIO_PORTF_PUR_R |= 0x10;				// enable weak pull-up on PF4
  GPIO_PORTF_IS_R &= ~0x10;					// PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x10;				// PF4 is not both edges
	GPIO_PORTF_IEV_R |= 0x10;					// PF4 is rising edge
		
	GPIO_PORTF_ICR_R = 0x10;      		// clear flag4
  GPIO_PORTF_IM_R |= 0x10;					// arm interrupt on PF4
		
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // priority 5
  NVIC_EN0_R = 0x40000000;					// enable interrupt 30 in NVIC  
	
}
void PortE_Button_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x00000010; // activate port E
	while ((SYSCTL_PRGPIO_R &0x10) == 0){};	// wait
		
  GPIO_PORTE_AMSEL_R &= ~0x4;// disable analog function on PE2
  //GPIO_PORTE_PCTL_R &= ~0x00FF0000; // configure PE2 as GPIO 
  GPIO_PORTE_DIR_R &= ~0x4;  // make PE2 in 
  GPIO_PORTE_AFSEL_R &= ~0x4;// disable alt funct on PE2 
  GPIO_PORTE_DEN_R |= 0x4;   // enable digital I/O on PE2
		
  GPIO_PORTE_IS_R &= ~0x4;   // PE2 is edge-sensitive 
  GPIO_PORTE_IBE_R &= ~0x4;  // PE2 is not both edges 
  GPIO_PORTE_IEV_R |= 0x4;   // PE2 rising edge event
		
  GPIO_PORTE_ICR_R = 0x4;    // clear flag5-4
  GPIO_PORTE_IM_R |= 0x4;    // enable interrupt on PE2
                              // GPIO PortE=priority 5
  NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF00)|0x000000A0; // bits 5-7
  NVIC_EN0_R = 0x10; // enable IRQ 4
	
}

void Button_Init(void){
	SW0D = 2;
  SW1D = 2;                    // clear semaphores
  SW2D = 2;
	PortC_Button_Init();
	PortF_Button_Init();
	PortE_Button_Init();
}