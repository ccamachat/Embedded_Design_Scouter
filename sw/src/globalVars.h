#include <stdint.h>

//define all flags here
//initialize the flags in LabX.c
// interrupts & timers will set the flags
//	main will constantly check for flags
extern uint32_t globalClock;

//alarmFlag
//alarmOffFlag for no sound
//alarmFlag for play sound
extern volatile uint32_t alarmFlag;

extern volatile uint32_t alarmOffFlag;

//alarmTime: time at which the alarm should sound
extern uint32_t alarm1Time;
extern uint32_t alarm2Time;

// redraw flag
extern volatile uint32_t redrawFlag;

// update mode flag
//extern volatile uint32_t modeFlag;

// select flag
extern volatile uint32_t selectFlag;

// set flag
extern volatile uint32_t setFlag;

// --------- Flags to Change Alarm Times --------- //
// Alarm 1
extern volatile uint32_t increment_Alarm1_Hour;
extern volatile uint32_t decrement_Alarm1_Hour;
extern volatile uint32_t increment_Alarm1_Minute;
extern volatile uint32_t decrement_Alarm1_Minute;

//Alarm 2
extern volatile uint32_t increment_Alarm2_Hour;
extern volatile uint32_t decrement_Alarm2_Hour;
extern volatile uint32_t increment_Alarm2_Minute;
extern volatile uint32_t decrement_Alarm2_Minute;

// ---------------------------------------------- //
//sound is playing
extern uint8_t alarmOn;
//disabled / enable
extern uint8_t alarm1Enable;
extern uint8_t alarm2Enable;

// ----------- MQTT Variables --------------- //

extern char w2b_cmd[2];

extern unsigned short   Hour;
extern unsigned short   Minute;
extern unsigned short   Second;

// -----------------------------Things actually in use rn ------------------------------//
// Main determinence
#define	LSM303AGR 0
#define	MAX30102 1
#define	FINAL 2
#define TEST 3
#define DISPLAY 4
#define MAIN TEST

// --------- Global Flags --------- //
extern volatile uint32_t changeModeFlag;
extern volatile uint32_t refreshDefaultFlag;
extern volatile uint32_t refreshCompassFlag;
extern volatile uint32_t refreshExerciseFlag;

// --------- Sub-Flags --------- //
extern volatile uint32_t updateStepFlag;
extern volatile uint32_t updateHeartrateFlag;

// ----------- Button Debouncing --------------- //
extern uint32_t SW0D, SW1D, SW2D;
