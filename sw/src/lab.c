/**
 * @file        lab.c
 * @author      your name (your_email@doman.com), Jonathan Valvano, Matthew Yu
 *              <TA NAME and LAB SECTION # HERE>
 * @brief       An empty main file for running your lab.
 * @version     0.1.0
 * @date        2022-10-08 <REPLACE WITH DATE OF LAST REVISION>
 * @copyright   Copyright (c) 2022
 * @note        Potential Pinouts:
 *                  Backlight (pin 10) connected to +3.3 V
 *                  MISO (pin 9) unconnected
 *                  SCK (pin 8) connected to PA2 (SSI0Clk)
 *                  MOSI (pin 7) connected to PA5 (SSI0Tx)
 *                  TFT_CS (pin 6) connected to PA3 (SSI0Fss)
 *                  CARD_CS (pin 5) unconnected
 *                  Data/Command (pin 4) connected to PA6 (GPIO)
 *                  RESET (pin 3) connected to PA7 (GPIO)
 *                  VCC (pin 2) connected to +3.3 V
 *                  Gnd (pin 1) connected to ground
 *
 *                  Center of 10k-ohm potentiometer connected to PE2/AIN1
 *                  Bottom of 10k-ohm potentiometer connected to ground
 *                  Top of 10k-ohm potentiometer connected to +3.3V
 *
 *              Warning. Initial code for the RGB driver creates bright flashing
 *              lights. Remove this code and do not run if you have epilepsy.
 */

/** File includes. */
#include <stdint.h>

/* Register definitions. */
#include "./inc/tm4c123gh6pm.h"
/* Clocking. */
#include "./inc/PLL.h"
/* Clock delay and interrupt control. */
#include "./inc/CortexM.h"
/* Initialization of all the pins. */
#include "./inc/Unified_Port_Init.h"
/* Talking to PC via UART. */
#include "./inc/UART.h"
/* ST7735 display. */
#include "./inc/ST7735.h"

/* Add whatever else you need here! */
#include "./lib/RGB/RGB.h"

int main(void) {
    /* Disable interrupts for initialization. */
    DisableInterrupts();

    /* Initialize clocking. */
    PLL_Init(Bus80Mhz);

    /* Allow us to talk to the PC via PuTTy! Check device manager to see which
       COM serial port we are on. The baud rate is 115200 chars/s. */
    UART_Init(7);

    /* Initialize all ports. */
    Unified_Port_Init();

    /* Start RGB flashing. WARNING! BRIGHT FLASHING COLORS. DO NOT RUN IF YOU HAVE EPILEPSY. */
    RGBInit();

    /* Allows any enabled timers to begin running. */
    EnableInterrupts();

    /* Print starting message to the PC and the ST7735. */
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_SetCursor(0, 0);
    ST7735_OutString(
        "ECE445L Final lab.\n"
        "Press SW1 to start.\n");
    UART_OutString(
        "ECE445L Final lab.\r\n"
        "Press SW1 to start.\r\n");
    Pause();

    /* Stop RGB and turn off any on LEDs. */
    RGBStop();
    PF1 = 0;
    PF2 = 0;
    PF3 = 0;

    /* Reset screen. */
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_SetCursor(0, 0);
    ST7735_OutString("Starting...\n");
    UART_OutString("Starting...\r\n");

    while (1) {
        /* TODO: Write your code here! */
        WaitForInterrupt();
    }
    return 1;
}

/** Function Implementations. */
void DelayWait10ms(uint32_t n) {
    uint32_t volatile time;
    while (n){
        time = 727240 * 2 / 91;  // 10msec
        while (time){
            --time;
        }
        --n;
    }
}

void Pause(void) {
    while (PF4 == 0x00) {
        DelayWait10ms(10);
    }
    while (PF4 == 0x10) {
        DelayWait10ms(10);
    }
}
