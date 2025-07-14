// Author : Ethan S
#ifndef __DISPLAY__
#define __DISPLAY__

// Information
// Amount of chars that fit per char size
// Assuming a horizontal ST7735 
// textSize - x * y
// 1 - 
// 2 - 13 chars * 6.5 chars 
// 3 - 9 chars  * 4.5 chars
// 4 - ~7 chars * 3.5 chars

// Scaled characters don't always align perfectly
// Use increment_XBuffer() & increment_YBuffer to adjust at your discretion
// X and Y buffers are sticky
// call reset_TextBuffers to clear

// control functions
void display_init(void);
void clear_screen(void);
void reset_TextBuffers(void);
void increment_XBuffer(void);
void increment_YBuffer(void);
void decrement_XBuffer(void);
void decrement_YBuffer(void);
void set_TextSize(uint8_t size);

// Initialization of each mode
void start_sequence(void);
void default_mode_init(void);
void compass_mode_init(void);
void exercise_mode_init(void);

// display functions

// Prints char by char with delay_ms * 10 ms between each char
void outputWithDelay(const char *str, uint32_t delay_ms);

// Must also include the cursor point you are printing from
// Only works with white text
void outputWithFade(const char *str,uint32_t cursorX, uint32_t cursorY);


#endif