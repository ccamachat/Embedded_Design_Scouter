#include <stdint.h>

extern uint8_t new_hours, new_minutes, new_seconds;

extern	uint32_t butSelect, currMode, soundOn, lightMode, setFlag, timerFlag, alarmFlag, hourMode, AMFlag;
extern uint32_t PB3State, PB4State, PB5State;

extern uint8_t alarm_hours1,alarm_hours2, alarm_minutes1, alarm_minutes2;
extern int8_t timer_hours1, timer_hours2, timer_minutes1, timer_minutes2, timer_seconds1, timer_seconds2;
 
extern char w2b_cmd[2];

extern uint16_t modeOut, hourOut, minOut, secOut;