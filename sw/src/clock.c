//Author : Ethan S.
#include "./clock.h"
#include "./globalVars.h"

/* ------ Dependent global vars ------ */
// Clock + alarm
uint32_t globalClock = 0;

uint32_t alarm1Time = 0;
uint32_t alarm2Time = 0;
uint8_t alarm1Enable = 0;
uint8_t alarm2Enable = 0;

// MQTT Vars
unsigned short Hour = 0;
unsigned short Second = 0;
unsigned short Minute = 0;

// Flags
uint32_t volatile redrawFlag = 0; // clk
//uint32_t volatile alarmFlag = 0;

/* ------------ Local Vars ------------ */
#define Timer3A_Period 80000000 // 1 sec
//#define Timer1A_Period 80000 // faster test freq.

// Clock Explanation :
//each 2^8 bits represents its respective hours minutes seconds
// [23:16] - hours
// [15:8] - minutes
// [7:0] - seconds
// format : 0x XX HH MM SS

void clock_Update(void) {
	int lastMin = 0, incMin;
	int lastSec = 0, incSec = 0;
	
	lastSec = getSeconds(globalClock);
	addSecond(&globalClock);
	incSec = getSeconds(globalClock);
	
	if(lastSec == 59 && incSec == 0){
		
		lastMin = getMinutes(globalClock);
		addMinute(&globalClock);
		incMin = getMinutes(globalClock);
		
		if(lastMin == 59 && incMin == 0){
			addHour(&globalClock);
		}
	}
	
	if(((globalClock >> 16) & 0xFF) == 24){
			globalClock = 0;
	}
	
	
	redrawFlag = 1;
	
	if(globalClock == alarm1Time && alarm1Enable){
		alarmFlag = 1;
	}
	if(globalClock == alarm2Time && alarm2Enable){
		alarmFlag = 1;
	}
	//MQTT Update
	Hour = getHours(globalClock);
	Minute = getMinutes(globalClock);
	Second = getSeconds(globalClock);
}

uint32_t getHours(uint32_t clock){ return (clock >> 16) & 0xFF;}
uint32_t getMinutes(uint32_t clock){ return (clock >> 8) & 0xFF;}
uint32_t getSeconds(uint32_t clock){ return clock  & 0xFF;}

void clearClock(void) { globalClock = 0; }

void addHour(uint32_t *clock){
	uint32_t temp;
	temp = getHours(*clock);
	if(temp==23) temp = 0;
	else temp++;
	*clock = (*clock & 0x00FFFF) + (temp << 16);
	/*
	temp = (getHours(*clock) + 1) % 24;
	*clock = (*clock & 0x00FFFF) + (temp << 16);
	*/
}

void addMinute(uint32_t *clock){
	uint32_t temp;
	temp = getMinutes(*clock);
	if(temp==59) temp = 0;
	else temp++;
	*clock = (*clock & 0xFF00FF) + (temp << 8);
	/*
	temp = (getMinutes(*clock) + 1) % 60;
	*clock = (*clock & 0xFF00FF) + (temp << 8);
	*/
}

void addSecond(uint32_t *clock){
	uint32_t temp;
	temp = getSeconds(*clock);
	if(temp==59) temp = 0;
	else temp++;
	*clock = (*clock & 0xFFFF00) + temp;
	/*
	temp = (getSeconds(*clock) + 1) % 60;
	*clock = (*clock & 0xFFFF00) + temp;
	*/
}

void subtractHour(uint32_t *clock){
	uint32_t temp;
	temp = getHours(*clock);
	if(temp==0) temp = 23;
	else temp = temp-1;
	*clock = (*clock & 0x00FFFF) + (temp << 16);
}

void subtractMinute(uint32_t *clock){
	uint32_t temp;
	temp = getMinutes(*clock);
	if(temp==0) temp = 59;
	else temp = temp-1;
	*clock = (*clock & 0xFF00FF) + (temp << 8);
}

void subtractSecond(uint32_t *clock){
	uint32_t temp;
	temp = getSeconds(*clock);
	if(temp==0) temp = 59;
	else temp = temp-1;
	*clock = (*clock & 0xFFFF00) + temp;
}

//inputs must be pre-sanitized
// hours: 0 - 23
// minutes: 0 - 59
// seconds: 0 - 59
void setTime(uint32_t *clock, uint8_t hours, uint8_t minutes, uint8_t seconds){
	*clock = 0;
	
	//set hours
	*clock += (hours << 16);
	//set minutes
	*clock += (minutes << 8);
	//set seconds
	*clock += seconds;
	
}

/* -------- Clock Timer ---------- */
void startClock(uint32_t clockPri){
	Timer3A_Init(&clock_Update, Timer3A_Period, clockPri);
};

void stopClock(void){
	Timer3A_Stop();
};
