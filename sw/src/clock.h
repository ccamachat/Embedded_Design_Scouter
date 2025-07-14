//Author: Ethan S
#include "stdint.h"
#include "../inc/Timer3A.h"


//Actions on clock x 
void clock_Update(void);

void addHour(uint32_t *clock);
void addMinute(uint32_t *clock);
void addSecond(uint32_t *clock);

void subtractHour(uint32_t *clock);
void subtractMinute(uint32_t *clock);
void subtractSecond(uint32_t *clock);

uint32_t getHours(uint32_t clock);
uint32_t getMinutes(uint32_t clock);
uint32_t getSeconds(uint32_t clock);

void startClock(uint32_t clockPri);
void stopClock(void);
void clearClock(void);

/**
 * @brief Will set the time of the clock
 * 
 * @param hours : 0 - 23
 * @param minutes :  0 - 59
 * @param seconds : 0 - 59 
 */
void setTime(uint32_t *clock, uint8_t hours, uint8_t minutes, uint8_t seconds);