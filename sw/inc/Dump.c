/**
 * @file Dump.c
 * @author your name (you@domain.com), Jonathan Valvano, Matthew Yu
 *    <TA NAME and LAB SECTION # HERE>
 * @brief 
 *    A set of debugging functions that capture data for later inspection.
 *    There are two primary methods:
 *       - DebugCapture will record data and time.
 *       - JitterMeasure will measure real time jitter.
 * @version 0.2
 * @date 2022-09-01 <REPLACE WITH DATE OF LAST REVISION>
 *
 * @copyright Copyright (c) 2022
 */

/** File includes. */
#include <stdint.h>
#include "../inc/Dump.h"
#include "../inc/tm4c123gh6pm.h"

// Global variables (Dump)
uint32_t DumpTimeBuffer[DUMPBUFSIZE];
uint32_t DumpDataBuf[DUMPBUFSIZE];
uint32_t DumpNum;

// Global variables (Jitter)
uint32_t MinElapsed;
uint32_t MaxElapsed;
uint32_t LastCallTime;

//Determines how many samples to skip at the start, if any, when calculating Jitter
#define JITTER_SKIP_COUNT 2
#define JITTER_TRIG_VALUE (0xFFFFFFFF-JITTER_SKIP_COUNT)

//Since some functions require the current time, we can setup a hardware timer to run
void Timer1_Init(void) {
    volatile uint32_t delay;
    SYSCTL_RCGCTIMER_R    |= 0x02;                // 0) activate TIMER1
    delay                  = SYSCTL_RCGCTIMER_R;  // allow time to finish activating
    TIMER1_CTL_R           = 0x00000000;          // 1) disable TIMER1A during setup
    TIMER1_CFG_R           = 0x00000000;          // 2) configure for 32-bit mode
    TIMER1_TAMR_R          = 0x00000002;          // 3) configure for periodic mode, COUNT-DOWN settings
    TIMER1_TAILR_R         = 0xFFFFFFFF;          // 4) reload value
    TIMER1_TAPR_R          = 0;                   // 5) bus clock resolution
    TIMER1_CTL_R           = 0x00000001;          // 10) enable TIMER1A
}

// What should dump init do?
// - reset the dump memory so we don't see old data.
// - reset or start any timing required for our DumpTimeBuffer.
void DumpInit(void){

    //Initalize global data structures before any useage occurs
    for(int i = 0; i < DUMPBUFSIZE; i++){
        DumpTimeBuffer[i] = 0;
        DumpDataBuf[i] = 0;
    }
    DumpNum = 0;

    //Start/Restart timer required for our DumpTimeBuffer.    
    Timer1_Init();
}

// Use TIMER1_TAR_R as current time.
// Note: how do you deal with timer overflow?
void DumpCapture(uint32_t data){

    // Only capture while there is space in the buffers, no overwriting
    if(DumpNum < DUMPBUFSIZE){  
        DumpDataBuf[DumpNum] = data;
        DumpTimeBuffer[DumpNum] = TIMER1_TAR_R;
        DumpNum += 1;
    }
}

uint32_t DumpCount(void){ 
    return DumpNum;
}

uint32_t* DumpData(void){
    return DumpDataBuf;
}

uint32_t* DumpTime(void){ 
    return DumpTimeBuffer;
}

// Note that jitter is independent of dump and should not 
// share the same data structures. Create and reset your own 
// variables indicating the measured jitter.
void JitterInit(void){    
    MinElapsed = 0xFFFFFFFF;
    MaxElapsed = 0;
    LastCallTime = 0;
    Timer1_Init();      //Note that this reset of the timer can cause odd behavior in your data dump
}

// We'll define the jitter we want to measure as "peak-to-peak" 
// jitter or "cycle-to-cycle" jitter; this is the maximum time 
// difference between any two periodic measurements.
//
// We determine this by measuring the time of successive calls:
// on the first time called, JitterMeasure just measures current 
// time. On subsequent calls, JitterMeasure measures the elapsed time
// from previous calls.
// 
// Note: how do you deal with timer overflow?
void JitterMeasure(void){
    
    //Record the the current time at start of call
    uint32_t currTime = TIMER1_TAR_R;

    if (MinElapsed > JITTER_TRIG_VALUE){
    
        // Technically reduces range of allowable periods compated to using a boolean flag
        MinElapsed--;

    } else {

        uint32_t currentElapsed = 0;

        if (currTime > LastCallTime){ 
            // Overflow Case
            currentElapsed = LastCallTime + (0xFFFFFFFF - currTime);
        } else {    
            // Normal Case
            currentElapsed = LastCallTime - currTime;
        }
        
        // checking if lower than min
        if (currentElapsed < MinElapsed){
            MinElapsed = currentElapsed;
        }
        //checking if higher than max
        if (currentElapsed > MaxElapsed){
            MaxElapsed = currentElapsed;
        }
        
    }

    // update the previous time
    LastCallTime = currTime;
}

uint32_t JitterGet(void){
    return MaxElapsed - MinElapsed;
}
