// -------------------------------------------------
// UART2.h
// Runs on TM4C123
// Simple device driver for the UART2 which is connected to debug port
//
// Author:      Daniel Valvano
// Date:        May 23, 2014
// Modified by: Mark McDermott
// Date:        May 25, 2018 
//
//-------------------------------------------------
//
// U2Rx connected to PD6
// U2Tx connected to PD7

// standard ASCII symbols
#define CR   0x0D
#define LF   0x0A
#define BS   0x08
#define ESC  0x1B
#define SP   0x20
#define DEL  0x7F

// initialize index FIFO

void UART2_Fifo_Init(uint32_t size);
	

//------------------- UART2_Init-------------------
// intializes uart and gpio needed to communicate with esp8266
// Configure UART2 for serial full duplex operation
// Inputs: none
// Outputs: none
void UART2_Init(void);


//------------   UART2_InChar   ------------
// Wait for new serial port input
// Input: none
// Output: ASCII code for key typed
//
char UART2_InChar(void);


//------------UART2_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
//
void UART2_OutChar(char data);

//---------------------UART2_Out_CRLF---------------------
// Output a CR,LF to UART to go to a new line
// Input: none
// Output: none
//
void UART2_Out_CRLF(void);


//----------UART2_FIFOtoBuffer----------
// - copies uart fifo to RXBuffer
// Inputs: none
// Outputs:none
void UART2_FIFOtoBuffer(void);


//----------UART2_GetString------------
// - gets data from software UART2 FIFO (in a string format)
// Inputs: pointer to empty buffer
// Outputs: Null terminated string
void UART2_GetString(unsigned char *bufPt);

//------------UART_InUDec------------
// InUDec accepts ASCII input in unsigned decimal format
//     and converts to a 32-bit unsigned number
//     valid range is 0 to 4294967295 (2^32-1)
// Input: none
// Output: 32-bit unsigned number
// If you enter a number above 4294967295, it will return an incorrect value
// Backspace will remove last digit typed
uint32_t UART2_InUDec(void);

//-----------------------UART_OutUDec-----------------------
// Output a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: none
// Variable format 1-10 digits with no space before or after
void UART2_OutUDec(uint32_t n);

//---------------------UART_InUHex----------------------------------------
// Accepts ASCII input in unsigned hexadecimal (base 16) format
// Input: none
// Output: 32-bit unsigned number
// No '$' or '0x' need be entered, just the 1 to 8 hex digits
// It will convert lower case a-f to uppercase A-F
//     and converts to a 16 bit unsigned number
//     value range is 0 to FFFFFFFF
// If you enter a number above FFFFFFFF, it will return an incorrect value
// Backspace will remove last digit typed
uint32_t UART2_InUHex(void);

//--------------------------UART_OutUHex----------------------------
// Output a 32-bit number in unsigned hexadecimal format
// Input: 32-bit number to be transferred
// Output: none
// Variable format 1 to 8 digits with no space before or after
void UART2_OutUHex(uint32_t number);


//------------UART_OutString------------
// Output String (NULL termination)
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void UART2_OutString(char *pt);


//------------UART_InString------------
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
void UART2_InString(char *bufPt, uint16_t max);


void Test_UART2(void);
