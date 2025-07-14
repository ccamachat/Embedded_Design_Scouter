// -----------------------------------------------------------
// UART2.c
// Runs on TM4C123
// Simple device driver for the UART2 which is connected to debug port
//
// Author:          Daniel Valvano
// Date:            May 23, 2014
//
// Modified by:     Mark McDermott
// Date:            Feb 13, 2019 
//
//-------------------------------------------------------------
//
// U2Rx connected to PD6
// U2Tx connected to PD7

#include <stdio.h>
#include <stdint.h>
#include "UART2.h"
#include "inc/tm4c123gh6pm.h"

#define UART2_FR_TXFF            0x00000020  // UART Transmit FIFO Full
#define UART2_FR_RXFE            0x00000010  // UART Receive FIFO Empty
#define UART2_LCRH_WLEN_8        0x00000060  // 8 bit word length
#define UART2_LCRH_FEN           0x00000010  // UART Enable FIFOs
#define UART2_CTL_UARTEN         0x00000001  // UART Enable



//-------------------   UART2_Init  -------------------
// Configure UART2 for serial full duplex operation
// Inputs: none
// Outputs: none

void UART2_Init(void) {

  SYSCTL_RCGCUART_R             |= 0x04;              // activate UART2
  while((SYSCTL_PRUART_R&0x04)==0){};                 // Wait
    
  SYSCTL_RCGCGPIO_R             |= 0x08;              // Enable PORT D clock gating
  while((SYSCTL_PRGPIO_R&0x08)==0){}; 
  
  GPIO_PORTD_LOCK_R              = 0x4C4F434B;        // unlock REQUIRED for PD7
  GPIO_PORTD_CR_R               |= 0xC0;              // commit PD6, PD7
  GPIO_PORTD_AFSEL_R            |= 0xC0;              // enable alternate function on PD6, PD7
  GPIO_PORTD_DEN_R              |= 0xC0;              // enable digital on PD6, PD7 (PD6 is RX, PD7 is TX)
  GPIO_PORTD_PCTL_R              =(GPIO_PORTD_PCTL_R 
                                &  0x00FFFFFF)
                                |  0x11000000;        // configure PD6, PD7 as UART
  
  GPIO_PORTD_AMSEL_R            &= ~0xC0;             // disable analog functionality on PD6, PD7
  UART2_CTL_R                   &= ~UART_CTL_UARTEN;  // Disable UART2 during config  
  
  // Setting Baud rate to 115200   
  UART2_IBRD_R                   = 43;                // IBRD = int(80,000,000 / (16 * 115,200)) = int(43.403)
  UART2_FBRD_R                   = 26;                // FBRD = round(0.4028 * 64 ) = 26

  UART2_LCRH_R                   = (UART_LCRH_WLEN_8 
                                |   UART_LCRH_FEN);   // 8 bit word length, 1 stop, no parity, FIFUART2 enabled
  UART2_IFLS_R                  &= ~0x3F;             // Clear TX and RX interrupt FIFO level fields
    
  //UART2_IFLS_R += UART_IFLS_RX1_8 ;                 // RX FIFO interrupt threshold >= 1/8th full
  //UART2_IM_R  |= UART_IM_RXIM | UART_IM_RTIM;       // Enable interupt on RX and RX transmission end
  //UART2: vector number 49 interrupt number 33  
  //NVIC_PRI12_R = (NVIC_PRI12_R&0xFFFF00FF)|(5<<13); //priority 5 (lowest priority higher than systick)
  //NVIC_EN1_R |= 1<<1;               	// enable IRQ 33 in NVIC
  
  UART2_CTL_R                   |= UART_CTL_UARTEN;      // Set UART2 enable bit  
}



//-----------------  UART2_InChar    ------------
// Wait for new serial port input
// Input: none
// Output: ASCII code for key typed
//

char UART2_InChar(void){
  while((UART2_FR_R&UART2_FR_RXFE) != 0);
  return((char)(UART2_DR_R & 0xFF));
}


//------------  UART2_OutChar     ------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
//
void UART2_OutChar(char data){
  while((UART2_FR_R&UART2_FR_TXFF) != 0);
  UART2_DR_R = data;
}

//----------  UART2_Out_CRLF  ---------------------
// Output a CR,LF to UART to go to a new line
// Input: none
// Output: none
//
void UART2_Out_CRLF(void){
  UART2_OutChar(CR);
  UART2_OutChar(LF);
}


//----------   UART2_OutString    ------------
// Output String (NULL termination)
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
//
void UART2_OutString(char *pt){
  while(*pt){
    UART2_OutChar(*pt);
    pt++;
  }
}

//------------UART2_InString------------
// Accepts ASCII characters from the serial port
//    and adds them to a string until <enter> is typed
//    or until max length of the string is reached.
// It echoes each character as it is inputted.
// If a backspace is inputted, the string is modified
//    and the backspace is echoed
// terminates the string with a null character
// uses busy-waiting synchronization on RDRF
// Input: pointer to empty buffer, size of buffer
// Output: Null terminated string
// -- Modified by Agustinus Darmawan + Mingjie Qiu --

void UART2_InString(char *bufPt, uint16_t max) {
int length=0;
char character;
  character = UART2_InChar();
  while(character != CR){
    if(character == BS){
      if(length){
        bufPt--;
        length--;
        UART2_OutChar(BS);
      }
    }
    else if(length < max){
      *bufPt = character;
      bufPt++;
      length++;
      UART2_OutChar(character);
    }
    character = UART2_InChar();
  }
  *bufPt = 0;
}


//------------UART2_InUDec------------
// InUDec accepts ASCII input in unsigned decimal format
// and converts to a 32-bit unsigned number
// valid range is 0 to 4294967295 (2^32-1)
// Input: none
// Output: 32-bit unsigned number
// If you enter a number above 4294967295, it will return an incorrect value
// Backspace will remove last digit typed
//

uint32_t UART2_InUDec(void){
uint32_t number=0, length=0;
char character;
  character = UART2_InChar();
  while(character != CR){             // accepts until <enter> is typed
    
// The next line checks that the input is a digit, 0-9.
// If the character is not 0-9, it is ignored and not echoed
    
    if((character>='0') && (character<='9')) {
      number = 10*number+(character-'0');   // this line overflows if above 4294967295
      length++;
      UART2_OutChar(character);
    }
    
// If the input is a backspace, then the return number is
// changed and a backspace is outputted to the screen
    
    else if((character==BS) && length){
      number /= 10;
      length--;
      UART2_OutChar(character);
    }
    character = UART2_InChar();
  }
  return number;
}

//-----------------------UART2_OutUDec-----------------------
// Output a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: none
// Variable format 1-10 digits with no space before or after
//

void UART2_OutUDec(uint32_t n){
// This function uses recursion to convert decimal number
//   of unspecified length as an ASCII string
//
  if(n >= 10){
    UART2_OutUDec(n/10);
    n = n%10;
  }
  UART2_OutChar(n+'0'); /* n is between 0 and 9 */
}


//---------------------UART2_InUHex----------------------------------------
// Accepts ASCII input in unsigned hexadecimal (base 16) format
// Input: none
// Output: 32-bit unsigned number
// No '$' or '0x' need be entered, just the 1 to 8 hex digits
// It will convert lower case a-f to uppercase A-F
//     and converts to a 16 bit unsigned number
//     value range is 0 to FFFFFFFF
// If you enter a number above FFFFFFFF, it will return an incorrect value
// Backspace will remove last digit typed
//
uint32_t UART2_InUHex(void){
uint32_t number=0, digit, length=0;
char character;
  character = UART2_InChar();
  while(character != CR){
    digit = 0x10; // assume bad
    if((character>='0') && (character<='9')){
      digit = character-'0';
    }
    else if((character>='A') && (character<='F')){
      digit = (character-'A')+0xA;
    }
    else if((character>='a') && (character<='f')){
      digit = (character-'a')+0xA;
    }
// If the character is not 0-9 or A-F, it is ignored and not echoed
    if(digit <= 0xF){
      number = number*0x10+digit;
      length++;
      UART2_OutChar(character);
    }
// Backspace outputted and return value changed if a backspace is inputted
    else if((character==BS) && length){
      number /= 0x10;
      length--;
      UART2_OutChar(character);
    }
    character = UART2_InChar();
  }
  return number;
}

//--------------------------UART2_OutUHex----------------------------
// Output a 32-bit number in unsigned hexadecimal format
// Input: 32-bit number to be transferred
// Output: none
// Variable format 1 to 8 digits with no space before or after
//

void UART2_OutUHex(uint32_t number){

	// This function uses recursion to convert the number of
	//   unspecified length as an ASCII string
  
  if(number >= 0x10){
    UART2_OutUHex(number/0x10);
    UART2_OutUHex(number%0x10);
  }
  else{
    if(number < 0xA){
      UART2_OutChar(number+'0');
     }
    else{
      UART2_OutChar((number-0x0A)+'A');
    }
  }
}

