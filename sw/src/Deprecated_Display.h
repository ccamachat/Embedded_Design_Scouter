#include <stdint.h>

void Increment_Seconds(uint8_t new_hours, uint8_t new_minutes, uint8_t new_seconds);
void Display_DefaultScreen (void);
void Display_TimerScreen (void);
void Increment_Clock_Display(void);
int Display_Flag_Value(void);
void Display_ClockScreen(void);

void Set_Display_Flag(int val);

//Display Menu will draw three segments at the bottom of the screen in yellow and red to indicate which option is selected
void Display_Menu(void);

//Converts the potentiometer to the menu item that is selected
void SelectedOption(uint32_t num);

//Select Digit sets the digit that the poten points to, to be incremented
void Select4Digit(uint32_t num);
void Select6Digit(uint32_t num);

void Display_DigitalClock(void);

//this will draw a small arrow below the alarm digit that you want to set
void DrawSelectedDigitAlarm(void);
void DrawSelectedDigitClock(void);
void DrawSelectedDigitTimer(void);

void Set_Display_Flag(int val);
void Initialize_Lookup_Table(void);

uint8_t Current_Display_Value(void);
void Set_Current_Display(uint8_t mode);
void Display_AlarmScreen(void);


void Initialize_Lookup_Table(void);

void Set_Current_Display(uint8_t mode);
void Toggle_LightMode(int data);

void Increment_4Digit(void);
void Increment_6Digit_Timer(void);
void Increment_6Digit_Clock(void);

void Increment_Hours_Web(void);
void Decrement_Hours_Web(void);
void Increment_Minutes_Web(void);
void Decrement_Minutes_Web(void);
void Increment_Seconds_Web(void);
void Decrement_Seconds_Web(void);
void Twelveto24();
void Twentyto12();
	