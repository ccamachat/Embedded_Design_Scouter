// Author : Ethan S

#include <string.h>
#include "../inc/ST7735.h"
#include "globalFunctions.h"
#include "Display.h"

// Information
// Amount of chars that fit per char size
// Assuming a horizontal ST7735 
// 128 high by 160 wide LCD
// textSize - x * y
// 1 - 20 chars * 15
// 2 - 13 chars * 6.5 chars 
// 3 - 9 chars  * 4.5 chars
// 4 - ~7 chars * 3.5 chars
// 5 - 5 * ~2.8
// 6 - 4.5 * ~2.3
// 7 - 4 * 2
// 8 - 3.5 * 1.75
// 9 - 3 * 1.5
// 10 - 3 * 1.4
// 11+ - Figure it out yourself (Increase cap of 10 in ST7735 code)
// **Note** 
//		- Could've used some exp decay function to model 
//				but am too lazy to figure it out w/o the use of float
//		- Take character limits with heavy grain of salt
//				Made measurements by eye

// Scaled characters don't always align perfectly
// Use increment_XBuffer() & increment_YBuffer to adjust at your discretion
// X and Y buffers are sticky
// call reset_TextBuffers to clear

#define roboticWait 19
#define fast 10
#define transitionSpeed 400
uint32_t fadeWait = 10;
#define animationSpeed 1

//Function Prototypes
void set_defaultText(void);


void reset_TextBuffers(){
	ST7735_ResetTextBuffers();
}

void increment_XBuffer(){
	ST7735_IncXBuf(-5);
}

void increment_YBuffer(){
	ST7735_IncYBuf(5);
}
void decrement_XBuffer(){
	ST7735_IncXBuf(5);
}
void decrement_YBuffer(){
	ST7735_IncYBuf(-5);
}
void set_TextSize(uint8_t size){
	ST7735_SetTextSize(size);
}
void display_init(void){
	ST7735_SetRotation(3);
	ST7735_SetTextColor(ST7735_WHITE);
	ST7735_SetTextSize(1);
	ST7735_Reflect();
	// Todo:
	// Change setCursor
}
void reset_defaultText(void){
	ST7735_SetTextSize(1);
	ST7735_SetTextColor(ST7735_WHITE);
}

void clear_screen(void){
	ST7735_FillScreen(ST7735_BLACK);
}



void start_sequence(void){
	ST7735_SetTextSize(2);
	//ST7735_SetTextColor(ST7735_GREEN);
	ST7735_SetTextColor(ST7735_WHITE);
	
	ST7735_SetCursor(2, 2);
	//outputWithDelay("Welcome", roboticWait);
	outputWithFade("Greetings", 2, 2);

	ST7735_SetCursor(3, 3);
	increment_XBuffer();
	//outputWithDelay("Saiyan", roboticWait);
	outputWithFade("Saiyan", 3, 3);
	fadeWait = animationSpeed;
	
	reset_TextBuffers();
	reset_defaultText();
}


// default mode includes 
// top left : heartbeat
// top right : steps
// bottom right : Weather
// bottom left : Time
void default_mode_init(void){
	ST7735_SetTextSize(2);
	ST7735_SetTextColor(ST7735_WHITE);
	
	ST7735_SetCursor(1, 2);
	outputWithDelay("Status Mode", fast);
	DelayWait10ms(transitionSpeed);
	clear_screen();
	
	ST7735_SetCursor(0,0);
	outputWithFade("HB: ", 0, 0);
	
	ST7735_SetCursor(11, 0);
	outputWithFade("00", 11, 0);
	
	increment_YBuffer();
	increment_YBuffer();

	ST7735_SetCursor(0, 5);
	outputWithFade("11:59 PM", 0, 5);
	
	ST7735_SetCursor(10, 5);
	outputWithFade("69", 10, 5);
	ST7735_OutChar(0xF7);
	
	reset_TextBuffers();
	reset_defaultText();
}

void refresh_default(void){
	// read different flags here as well
	// this will allow for asynchronous updates of stats
}

// Top 1/3 Compass header drawing
// Bottom Left : Elevation
// Bottom Right : City, State
void compass_mode_init(void){
	ST7735_SetTextSize(2);
	ST7735_SetTextColor(ST7735_WHITE);
	
	ST7735_SetCursor(0, 2);
	outputWithDelay("Compass Mode", fast);
	DelayWait10ms(transitionSpeed);
	clear_screen();
	
	ST7735_SetTextSize(1);
	// initialize compass heading 
	// write logic
	ST7735_SetCursor(0,11); 
	outputWithFade("Elev. 440ft", 0 , 11);
	
	ST7735_SetCursor(0, 12);
	outputWithFade("Austin, TX", 0, 12);
	
	reset_TextBuffers();
	reset_defaultText();
}
void refresh_compass(void){
	// same as refresh_default
	// read some flags
	
}
// Top 1/3 Heartbeat plotting  + BPM
// Bottom Left:
//	- SpO2 : XX%
//  - Bdy Temp : XXX
void exercise_mode_init(void){
	ST7735_SetTextSize(2);
	ST7735_SetTextColor(ST7735_WHITE);
	
	ST7735_SetCursor(0, 2);
	outputWithDelay("Exercise Mode", fast);
	DelayWait10ms(transitionSpeed);
	clear_screen();
	
	
	//initialize the graph here
	//initialize the bpm next to graph here
	increment_YBuffer();
	increment_YBuffer();
	
	ST7735_SetCursor(0,4);
	outputWithFade("SpO2:", 0, 4);
	
	ST7735_SetCursor(0,5);
	outputWithFade("Bdy Temp: 80", 0, 5);
	
	reset_defaultText();
	reset_TextBuffers();
	
}

// Functions
void outputWithDelay(const char *str, uint32_t delay_ms) {
    // Calculate the length of the string
    size_t len = strlen(str);

    // Iterate over each character of the string
    for (size_t i = 0; i < len; i++) {
        // Output the current character using ST7735_OutChar
        ST7735_OutChar(str[i]);

        // Wait for the specified delay
				DelayWait10ms(delay_ms); 
    }
}

// Only works for the color white
// If you want faster decrease the ratio between loop iterations : r,g,b increments
void outputWithFade(const char *str, uint32_t cursorX, uint32_t cursorY){
	uint8_t r = 5;
	uint8_t g = 5;
	uint8_t b = 5;
	size_t len = strlen(str);
	
	uint16_t currentColor = ST7735_Color565(r, g, b);
	for(int i = 0; i < 25; i++){
		ST7735_SetTextColor(currentColor);
		ST7735_SetCursor(cursorX, cursorY);
		for (size_t i = 0; i < len; i++) {
			ST7735_OutChar(str[i]);
		}
		r = (r+10 <= 255)? r+10: 255;
		g = (g+10 <= 255)? r+10: 255;
		b = (b+10 <= 255)? r+10: 255;
		currentColor = ST7735_Color565(r, g, b);
		DelayWait10ms(fadeWait);
	}
}