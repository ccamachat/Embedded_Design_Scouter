//PF4 is SW1(left) and PF0 is SW0 (right) are the on board switches

#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>
#include "../inc/ST7735.h"
#include "Lab5.h"

uint32_t PB3State = 0, PB4State = 0, PB5State = 0; 

//TODO: initialize on board switches
void Deprecated_Button_Init(void){volatile long delay;                            
  SYSCTL_RCGCGPIO_R |= 0x02;           // activate port B
  while((SYSCTL_PRGPIO_R&0x10)==0){}; // allow time for clock to start
  delay = 100;                  //    allow time to finish activating
//  GPIO_PORTE_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port E 
//  GPIO_PORTE_CR_R = 0x0F;           // allow changes to PE3-0                              // 2) GPIO Port E needs to be unlocked
  GPIO_PORTB_AMSEL_R &= ~0x38;  // 3) disable analog on PB5-3

  GPIO_PORTB_PCTL_R &= ~0x00FF0000; //was 0x000F000F before change to E    <--cahnge
  GPIO_PORTB_DIR_R &= ~0x38;    // 5) make PB5-3 in
  GPIO_PORTB_AFSEL_R &= ~0x38;  // 6) disable alt funct on PE5-3
//	GPIO_PORTE_PUR_R |= 0x03;     
  GPIO_PORTB_DEN_R |= 0x38;     // 7) enable digital I/O on PB5-3
  GPIO_PORTB_IS_R &= ~0x38;     //    PB5-3 is edge-sensitive
  GPIO_PORTB_IBE_R &= ~0x38;    //    PB5-3 is not both edges
  //GPIO_PORTB_IEV_R &= ~0x38;     //    PB5-3 falling edge event (Neg logic)
	GPIO_PORTB_IEV_R |= 0x38;     //    PB5-3 falling edge event (Neg logic)
  GPIO_PORTB_ICR_R = 0x38;      //    clear flag1-0
  GPIO_PORTB_IM_R |= 0x38;      // 8) arm interrupt on PB5-3
                                // 9) GPIOF priority 2
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFFFF00FF)|0x00006000; //was &0xFF0FFFFF)|
  NVIC_EN0_R = 0x2;   // 10)enable interrupt 4 in NVIC 
}
//TODO: write proper handler for different switches
//void GPIOPortB_Handler(void){
//	if ((GPIO_PORTB_RIS_R & 0x08) == 0x08){ //PB3 --> OK button
//		
//		PB3State = GPIO_PORTB_DATA_R & 0x08;
//		DelayWait10ms(10);
//		//delay and check for bouncing
//		
//		if (PB3State != ( GPIO_PORTB_DATA_R & 0x08) && (GPIO_PORTB_DATA_R & 0x08) == 0x08){
//			
//		}
//		//bounced
//		else GPIO_PORTB_ICR_R = 0x08; //acknowledge the flag even if bouncing	
//	}
//		
//	
//	else if ((GPIO_PORTB_RIS_R & 0x10)==0x10){ //PB4 -> increment digit. 
//		
//		PB4State = GPIO_PORTB_DATA_R & 0x10;
//		DelayWait10ms(10);
//		// for debouncing
//		if (PB4State != ( GPIO_PORTB_DATA_R & 0x10) && (GPIO_PORTB_DATA_R & 0x10) == 0x10){ //incerment

//		}
//		else GPIO_PORTB_ICR_R = 0x10; //acknowledge the flag
//	}
//	
//	
//	else if ((GPIO_PORTB_RIS_R & 0x20)==0x20){ //PB5 -- Snooze/Sound off button
//		
//		PB5State = GPIO_PORTB_DATA_R & 0x20;
//		DelayWait10ms(10);
//		
//		if (PB5State != ( GPIO_PORTB_DATA_R & 0x20) && (GPIO_PORTB_DATA_R & 0x20) == 0x20){
//		}
//		else GPIO_PORTB_ICR_R = 0x20; //acknowledge the flag
//	}
//	
//}
