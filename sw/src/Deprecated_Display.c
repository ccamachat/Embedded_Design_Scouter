//for graphic display functions on ST7335
//TODO: write functions to display the clock face, timer, etc. 
//Funny picture when alarm is completed
#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>
#include <stdlib.h>
#include "../inc/ST7735.h"
#include "../inc/hint_7b.txt"
#include <math.h>
#include "globalVariables.h" 

#define PI 3.14159265358979323846
#define CLOCK_CENTER_X 62
#define CLOCK_CENTER_Y 80
#define CLOCK_RADIUS 30
#define LOOKUP_TABLE_SIZE 60 //THIS WILL HAVE VALUES OF ALL QUADRANTS!!! 

//X/Y lookup table calculated outside of program
//rows contain 15 positions each
uint16_t X_LOOKUP[60] = {62, 65, 68, 71, 74, 77, 79, 82, 84, 86, 87, 89, 90, 91, 91,
                        92, 91, 91, 92, 89, 87, 86, 84, 82, 79, 77, 74, 71, 68, 65, 
                        62, 58, 55, 52, 49, 47, 44, 41, 39, 37, 36, 34, 33, 33, 32, 
                        32, 32, 33, 33, 34, 36, 37, 39, 41, 44, 47, 49, 52, 55, 58};

												
uint16_t Y_LOOKUP[60] = {50, 50, 50, 51, 52, 54, 55, 57, 59, 62, 65, 67, 70, 73, 76,
                        80, 83, 86, 89, 92, 95, 97, 100, 102, 104, 105, 107, 108, 109, 109,
                        110, 109, 109, 108, 107, 105, 104, 102, 100, 97, 95, 92, 89, 86, 83,
                        80, 76, 73, 70, 67, 65, 62, 59, 57, 55, 54, 52, 51, 50, 50};

// More lookup tables for x and y hour hand									
uint16_t X_HOUR_LOOKUP[60] = {62, 63, 65, 66, 68, 69, 70, 72, 73, 74, 74, 75, 76, 76, 76, 
	                            77, 76, 76 ,76 ,75 ,74, 74, 73, 72, 70, 69, 68, 66, 65, 63, 
	                            62, 60, 58, 57, 55, 54, 53, 51, 50 , 49, 49, 48, 47, 47, 47, 
                              47, 47, 47, 47, 48, 49, 49, 50 ,51, 53, 54, 55, 57, 58, 60};

uint16_t Y_HOUR_LOOKUP[60] = {65, 65, 65, 65, 66, 67, 67, 68, 69, 71, 72, 73, 75, 76, 78,
	                            80, 81, 83, 84, 86, 87, 88, 90 ,91, 92, 92, 93, 94, 94, 94,
                             	95, 94, 94, 94, 93, 92, 92, 91, 90, 88, 87, 86, 84, 83, 81,
                              80, 78, 73, 75, 73, 72, 71, 69, 68, 67, 67, 66, 65, 65, 65};

// variable used to store which menu option has been selected
uint32_t selectedButton, selected4Digit, selected6Digit;


//Default screen will include the clock face, an analog description of the time, and a menu to select modes

		static uint8_t hours1,		hours2, 		minutes1 , 		minutes2 , 		seconds1 , 		seconds2 ;
		static uint8_t hours, minutes, seconds;
    static int x_Lookup_Table[LOOKUP_TABLE_SIZE];
		static int y_Lookup_Table[LOOKUP_TABLE_SIZE];
		static uint8_t minuteCounter = 0;
		static int DisplayFlag = 0; 
void Display_DefaultScreen (void) { //this will flash for 3 seconds. then proceed to clock screen :3
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_SetCursor(0,0);
	ST7735_OutString("Please use slidepot");
	ST7735_OutChar(13);
	ST7735_OutString("to move cursor");
	ST7735_OutChar(13);
	ST7735_OutString("and Key0 to select");
	ST7735_SetCursor(3,5);
	//next screen??
}




// This function will print the current display to the ST7735 when fed the Current Mode
// There should be 4 displays: Main Clock, Alarm Set, Timer Set, Temperature 
void Set_Current_Display(uint8_t mode){
	currMode = mode;
	
}
uint8_t Current_Display_Value(void){
	return currMode;
}

void DrawSelectedDigitAlarm(void){
	ST7735_SetTextColor(ST7735_RED);
	ST7735_SetCursor(7,5);
	switch(selected4Digit){
		case 0: ST7735_OutString("^    "); break;
		case 1: ST7735_OutString(" ^   "); break;
		case 2: ST7735_OutString("   ^ "); break;
		case 3: ST7735_OutString("    ^"); break;
	}		
			
	ST7735_SetTextColor(ST7735_YELLOW);
}

void DrawSelectedDigitClock(void){
	ST7735_SetTextColor(ST7735_RED);
	ST7735_SetCursor(7,2);
	switch(selected6Digit){
		case 0: ST7735_OutString("^       "); break;
		case 1: ST7735_OutString(" ^      "); break;
		case 2: ST7735_OutString("   ^    "); break;
		case 3: ST7735_OutString("    ^   "); break;
		case 4: ST7735_OutString("      ^ "); break;
		case 5: ST7735_OutString("       ^"); break;
	}		
			
	ST7735_SetTextColor(ST7735_YELLOW);
}

void DrawSelectedDigitTimer(void){
	ST7735_SetTextColor(ST7735_RED);
	ST7735_SetCursor(7,5);
	switch(selected6Digit){
		case 0: ST7735_OutString("^       "); break;
		case 1: ST7735_OutString(" ^      "); break;
		case 2: ST7735_OutString("   ^    "); break;
		case 3: ST7735_OutString("    ^   "); break;
		case 4: ST7735_OutString("      ^ "); break;
		case 5: ST7735_OutString("       ^"); break;
	}		
			
	ST7735_SetTextColor(ST7735_YELLOW);
}

// This function will print the current display to the ST7735 when fed the Current Mode
// There should be 4 displays: Main Clock, Alarm Set, Timer Set, Temperature 


void Display_ClockScreen(void){

	//ST7735_FillScreen(ST7735_BLACK);
	
	
		//draw the clock face right under.

		
		ST7735_DrawBitmap(14,130, hint_7b, 100, 100); //(160*128)
		

}
void Display_TimerScreen (void) {
	
		ST7735_SetCursor(7,4); //newX, newY
		
		char timerString[9];
		timerString[0] = '0' + timer_hours1;
		timerString[1] = '0' + timer_hours2;
		timerString[2] = ':';
		timerString[3] = '0' + timer_minutes1;
		timerString[4] = '0' + timer_minutes2;
		timerString[5] = ':';
		timerString[6] = '0' + timer_seconds1;
		timerString[7] = '0' + timer_seconds2;
		timerString[8] = '\0';
		//change this to drawchar to make it bigger!
		ST7735_OutString(timerString);
		//DrawSelectedDigitTimer(); //--> put in Set Timer

}
void Display_AlarmScreen(void){
	//display the actual time. 

		ST7735_SetCursor(7,4); //newX, newY
		
		char alarmString[6];
		alarmString[0] = '0' + alarm_hours1;
		alarmString[1] = '0' + alarm_hours2;
		alarmString[2] = ':';
		alarmString[3] = '0' + alarm_minutes1;
		alarmString[4] = '0' + alarm_minutes2;
		alarmString[5] = '\0';
		//change this to drawchar to make it bigger!
		ST7735_OutString(alarmString);
		//DrawSelectedDigitAlarm(); //--> put in setAlarm
}

void Toggle_LightMode(int data){
	ST7735_InvertDisplay(data);
}

void Display_Menu(void){
	if (selectedButton == 0){
		ST7735_DrawFastHLine(0, 140, 42, ST7735_RED);
		ST7735_DrawFastHLine(42, 140, 86, ST7735_YELLOW);
		ST7735_DrawFastVLine(42, 140, 20, ST7735_RED);
		ST7735_DrawFastVLine(80, 140, 20, ST7735_YELLOW);
	}
	else if (selectedButton == 1){
		ST7735_DrawFastHLine(0, 140, 42, ST7735_YELLOW);
		ST7735_DrawFastHLine(42, 140, 38, ST7735_RED);
		ST7735_DrawFastHLine(80, 140, 48, ST7735_YELLOW);
		ST7735_DrawFastVLine(42, 140, 20, ST7735_RED);
		ST7735_DrawFastVLine(80, 140, 20, ST7735_RED);
	}
	else{
		ST7735_DrawFastHLine(0, 140, 80, ST7735_YELLOW);
		ST7735_DrawFastHLine(80, 140, 48, ST7735_RED);
		ST7735_DrawFastVLine(42, 140, 20, ST7735_YELLOW);
		ST7735_DrawFastVLine(80, 140, 20, ST7735_RED);
	}
		ST7735_SetCursor(1,15);
		ST7735_OutString("MODE");
		ST7735_SetCursor(9,15);
		ST7735_OutString("SET");
		ST7735_SetCursor(15,15);
		ST7735_OutString("THEME");
}



// Pass this function the slide poten vlaue to use to display the currently selected menu option
void SelectedOption(uint32_t num){
	if (num == 0) selectedButton = 0;
	else if (num == 3) selectedButton = 2;
	else selectedButton = 1;
}

// Pass this function the slide poten value to use to display the currently selected digit (for alarm set and timer)
void Select4Digit(uint32_t num) {
	selected4Digit = num; 
}
void Select6Digit(uint32_t num){
	selected6Digit = num;
}

void Display_DigitalClock(void){
		ST7735_SetCursor(7,1); //newX, newY
	
	char timeString[11];  // Assuming HH:MM:SS format, 8 characters + null-terminator

    // Convert hours to characters

    timeString[0] = '0' + hours1;
    timeString[1] = '0' + hours2;
    timeString[2] = ':';

    // Convert minutes to characters
    timeString[3] = '0' + minutes1;
    timeString[4] = '0' + minutes2;
    timeString[5] = ':';

    // Convert seconds to characters
    timeString[6] = '0' + seconds1;
    timeString[7] = '0' + seconds2;
		if (hourMode == 0x0){
			if(AMFlag==0){
				timeString[8] = 'A';
				timeString[9] = 'M';
				}
			else if (AMFlag ==1){
				timeString[8] = 'P';
				timeString[9] = 'M';
				}
		}
		else {
			timeString[8] = ' ';
			timeString[9] = ' ';
		}
    // Null-terminate the string
    timeString[10] = '\0';
		ST7735_OutString(timeString);
		
		//DrawSelectedDigitClock();  // Put in Set clock
}
void Increment_Seconds(uint8_t New_hours, uint8_t New_minutes, uint8_t New_seconds){ //the display clock  values!
					if (hourMode == 0x0) { // 12 hour mode
					hours = New_hours;
					} else if (hourMode == 0x1) { // 24 hour mode
					if (AMFlag == 1) { // PM
						hours = (New_hours + 12) % 24;
						} else { // AM
						hours = New_hours % 24;
						}
					}
					minutes = New_minutes;
					seconds = New_seconds;

					// Separate hours, minutes, and seconds into tens and units digits
					hours1 = hours / 10;
					hours2 = hours % 10;
					minutes1 = minutes / 10;
					minutes2 = minutes % 10;
					seconds1 = seconds / 10;
					seconds2 = seconds % 10;

						DisplayFlag = 1;
	//set a flag in main. this will trigger a display change. 

}
double degreesToRadians(double degrees) {
    return degrees * PI / 180.0;
}
void Increment_Clock_Display(void){
	//adjust the clock hands!!
	//only first qudrant.
	int hourAngle = (30) * (hours % 12) + (6 * minutes / 12) % 45;
	int minuteAngle = (6 * minutes) % 45;
	int secondAngle = (6 * seconds) % 45; //will only go from 0 to 45

    // Calculate end points of clock hands using the lookup tables
    //int hourHandX = CLOCK_CENTER_X + SineLookupTable[hourAngle / 6]; //0, 6, 12, 18
    //int hourHandY = CLOCK_CENTER_Y - abs(SineLookupTable[(90 - hourAngle) / 6]);

   // int minuteHandX = CLOCK_CENTER_X + SineLookupTable[minuteAngle / 6];
    //int minuteHandY = CLOCK_CENTER_Y - SineLookupTable[(90 - minuteAngle) /6];
	// only care about this rn. make different CASES FOR SECONDS VALUE
	

		
	int secondHandX = X_LOOKUP[seconds];
	int secondHandY = Y_LOOKUP[seconds];
	int minuteHandX  = X_LOOKUP[minutes];
	int minuteHandY = Y_LOOKUP[minutes];
	//int hourHandX = X_HOUR_LOOKUP[hours];
	//int hourHandY = Y_HOUR_LOOKUP[ hours];
		//minutes/12 will go from 0 to 5. 
		int hourHandX = X_HOUR_LOOKUP[minutes/12 +5*(hours%12)];
		int hourHandY = Y_HOUR_LOOKUP[minutes/12 +5*(hours%12)];
	

    // Draw clock hands using the calculated end points
    // (You need to implement your drawing function here)
    
		ST7735_Line(CLOCK_CENTER_X, CLOCK_CENTER_Y, hourHandX, hourHandY, ST7735_WHITE);
		
    ST7735_Line(CLOCK_CENTER_X, CLOCK_CENTER_Y, minuteHandX, minuteHandY, ST7735_WHITE);
    ST7735_Line(CLOCK_CENTER_X, CLOCK_CENTER_Y, secondHandX, secondHandY, ST7735_RED);

	
}
void Initialize_Lookup_Table(void){
for (int angle = 0; angle <16; angle++) { //15 entries... 6 degrees for each. will cover one quadrant

				x_Lookup_Table[angle] = CLOCK_CENTER_X + (int)(CLOCK_RADIUS * sin(degreesToRadians(angle * 6)));
				y_Lookup_Table[angle] = CLOCK_CENTER_Y - (int)(CLOCK_RADIUS * cos(degreesToRadians(angle * 6)));
	
}

for (int angle = 16; angle <31; angle++) { //15 entries... 6 degrees for each. will cover one quadrant
        
				x_Lookup_Table[angle] = x_Lookup_Table[15-(angle%16)];
				y_Lookup_Table[angle] = CLOCK_CENTER_Y -y_Lookup_Table[15-(angle%16)]+ CLOCK_CENTER_Y;
}
for (int angle = 31; angle <46; angle++) { //15 entries... 6 degrees for each. will cover one quadrant
        //SineLookupTable[angle] = ;  // 6 degrees per second/minute
				x_Lookup_Table[angle] = CLOCK_CENTER_X+CLOCK_CENTER_X- x_Lookup_Table[15-(angle%16)];
				y_Lookup_Table[angle] = CLOCK_CENTER_Y - y_Lookup_Table[angle%15]+CLOCK_CENTER_Y ;
}
for (int angle = 46; angle <60; angle++) { //15 entries... 6 degrees for each. will cover one quadrant
        //SineLookupTable[angle] = ;  // 6 degrees per second/minute
				x_Lookup_Table[angle] = -CLOCK_RADIUS+x_Lookup_Table[15-(angle%16)];
				y_Lookup_Table[angle] =  y_Lookup_Table[15-(angle%16)];
} 
//seconds will NEVER EQUAL 60....
	
}

	
int Display_Flag_Value(void){
	return DisplayFlag;
}
void Set_Display_Flag(int val){
	DisplayFlag = val;
}
void Increment_Hours_Web(void){
    if(hourMode == 0){ // 12 hour mode
        if(currMode == 0){ // clock mode
            hours2++;
            if(hours1 == 1 && hours2 == 3){ // goes past 12.
                hours1 = 0;
                hours2 = 1;
            } else if(hours2 == 10){ // goes to past 9
                hours1 = 1;
                hours2 = 0;
            }
						if (AMFlag == 0 && hours1 ==1 && hours2 == 2) { //if its 12 am...
                    AMFlag = 1; // Switch from AM to PM
                } else if (AMFlag == 1 && hours2 == 2) {
                    AMFlag = 0; // Switch from PM to AM
                }
            new_hours = hours1 * 10 + hours2;
        } else if(currMode == 1){ // alarm mode
            alarm_hours2++;
            if(alarm_hours1 == 1 && alarm_hours2 == 3){ // goes past 12.
                alarm_hours1 = 0;
                alarm_hours2 = 1;
            } else if(alarm_hours2 == 10){ // goes to past 9
                alarm_hours1 = 1;
                alarm_hours2 = 0;
            }
        } else if (currMode == 2){ // timer mode
            timer_hours2++;
            if(timer_hours2 == 10){ // goes to past 9
                timer_hours1++;
                timer_hours2 = 0;
            }
        }
    } else if (hourMode == 1){ // 24 hour mode
        if(currMode == 0){ // clock mode
            hours2++;
            if(hours1 == 2 && hours2 == 4){ // goes past 24, loops back to 0
                hours1 = 0;
                hours2 = 0;
            } else if(hours2 == 10){ // goes to past 9
                hours1 = hours1 + 1;
                hours2 = 0;
            }
            new_hours = hours1 * 10 + hours2;
        } else if (currMode == 1){ // alarm mode
            alarm_hours2++;
            if(alarm_hours1 == 2 && alarm_hours2 == 4){ // goes past 24.
                alarm_hours1 = 0;
                alarm_hours2 = 0;
            } else if(alarm_hours2 == 10){ // goes to past 9
                alarm_hours1 = alarm_hours1 + 1;
                alarm_hours2 = 0;
            }
        } else if (currMode == 2){ // timer mode
            timer_hours2++;
            if(timer_hours2 == 10){ // goes to past 9
                timer_hours1++;
                timer_hours2 = 0;
            }
        }
    }
}
void Decrement_Hours_Web(void){
    if (hourMode == 0) { // 12-hour mode
        if (currMode == 0) { // clock mode 
					if (hours1 ==1 && hours2 == 2){
								if(AMFlag ==0){ //toggle AM Flag
								AMFlag =1;
								}
								else if (AMFlag ==1){
									AMFlag =0;
								}
						}
						if (hours1 == 0 && hours2 == 1) { // goes from 1 to 12
                hours1 = 1;
                hours2 = 2;

            }  else if (hours1 == 1 && hours2 == 0) { // goes from 10 to 9
                hours1 = 0;
                hours2 = 9;
            }						
						else {
                hours2--; //YAYYY
            }					
            new_hours = hours1 * 10 + hours2;
        } else if (currMode == 1) { // alarm mode
            if (alarm_hours1 == 0 && alarm_hours2 == 1) { // goes from 1 to 12
                alarm_hours1 = 1;
                alarm_hours2 = 2;
            }  else if (alarm_hours1 == 1 && alarm_hours2 == 0) { // goes from 10 to 9
                alarm_hours1 = 0;
                alarm_hours2 = 9;
            } else {
                alarm_hours2--; //YAYYY
            }
        } else if (currMode == 2) { // timer mode
            if (timer_hours2 == 0) { 
                timer_hours1--;
                timer_hours2 = 9;
            }  else {
                timer_hours2--;
            }
        }
    } else if (hourMode == 1) { // 24-hour mode
        if (currMode == 0) { // clock mode

        } else if (currMode == 1) { // alarm mode

    }
	}
}
void Increment_Minutes_Web(void){
	if (currMode == 0) { // clock mode
        if (minutes2 == 9) {
            minutes2 = 0;
            if (minutes1 == 5) {
                minutes1 = 0;
            } else {
                minutes1++;
            }
        } else {
            minutes2++;
        }
				new_minutes = minutes1*10 +minutes2;
    } else if (currMode == 1) { // alarm mode    
			        if (alarm_minutes2 == 9) {
            alarm_minutes2 = 0;
            if (alarm_minutes1 == 5) {
                alarm_minutes1 = 0;
            } else {
                alarm_minutes1++;
            }
        } else {
            alarm_minutes2++;
        }
    } else if (currMode == 2) { // timer mode
			       if (timer_minutes2 == 9) {
            timer_minutes2 = 0;
            if (timer_minutes1 == 5) {
                timer_minutes1 = 0;
            } else {
                timer_minutes1++;
            }
        } else {
            timer_minutes2++;
        }
    }
	
}
void Decrement_Minutes_Web(void){
	if (currMode == 0) { // clock mode
        // Decrement minutes for clock mode
        if (minutes2 == 0) {
            minutes2 = 9;
            if (minutes1 == 0) {
                minutes1 = 5;
            } else {
                minutes1--;
            }
        } else {
            minutes2--;
        }
				new_minutes = minutes1*10 +minutes2;
    } else if (currMode == 1) { // alarm mode
			        if (alarm_minutes2 == 0) {
            alarm_minutes2 = 9;
            if (alarm_minutes1 == 0) {
                alarm_minutes1 = 5;
            } else {
                alarm_minutes1--;
            }
        } else {
            alarm_minutes2--;
        }

    } else if (currMode == 2) { // timer mode
			        if (timer_minutes2 == 0) {
            timer_minutes2 = 9;
            if (timer_minutes1 == 0) {
                timer_minutes1 = 5;
            } else {
                timer_minutes1--;
            }
        } else {
            timer_minutes2--;
        }

    }
}
void Increment_Seconds_Web(void){
	if (currMode == 0) { // clock mode
        if (seconds2 == 9) {
            seconds2 = 0;
            if (seconds1 == 5) {
                seconds1 = 0;
            } else {
                seconds1++;
            }
        } else {
            seconds2++;
        }
				new_seconds = seconds1*10 +seconds2;
    }  else if (currMode == 2) { // timer mode
				if (timer_seconds2 == 9) {
            timer_seconds2 = 0;
            if (timer_seconds1 == 5) {
                timer_seconds1 = 0;
            } else {
                timer_seconds1++;
            }
        } else {
            timer_seconds2++;
        }
    }
	
	
}
void Decrement_Seconds_Web(void){
	if (currMode == 0) { // clock mode
        if (seconds2 == 0) {
            seconds2 = 9;
            if (seconds1 == 0) {
                seconds1 = 5;
            } else {
                seconds1--;
            }
        } else {
            seconds2--;
        }
				new_seconds = seconds1*10 +seconds2;
    }  else if (currMode == 2) { // timer mode
			if (timer_seconds2 == 0) {
            timer_seconds2 = 9;
            if (timer_seconds1 == 0) {
                timer_seconds1 = 5;
            } else {
                timer_seconds1--;
            }
        } else {
            timer_seconds2--;
        }

    }
}
void Increment_6Digit_Clock(void){
	if (selected6Digit == 0) {
       if (hourMode == 0) { // 12-hour mode
            hours1++;
            if (hours1 > 1) {
                hours1 = 0; // Reset hours1 to 0 when it gets to 2
            }
        } else if (hourMode == 1) { // 24-hour mode
            hours1++;
            if (hours1 > 2) {
                hours1 = 0; // Reset hours1 to 0 when it gets to 3
            }
        }
    }
    else if (selected6Digit == 1) {
        hours2++;
        if (hours2 > 9) {
            hours2 = 0; // Reset hours2 to 0 when it gets to 3 
        }
    }
    else if (selected6Digit == 2) {
        minutes1++;
        if (minutes1 > 5) {
            minutes1 = 0; // Reset minutes1 to 0 when it gets to 6
        }
    }
    else if (selected6Digit == 3) {
        minutes2++;
        if (minutes2 > 9) {
            minutes2 = 0; // Reset minutes2 to 0 when it gets to 10
        }
    }
		else if (selected6Digit == 4) {
        seconds1++;
        if (seconds1 > 5) {
            seconds1 = 0; // Reset seconds1 to 0 when it gets to 6
        }
    }
		else if (selected6Digit == 5) {
        seconds2++;
        if (seconds2 > 9) {
            seconds2 = 0; // Reset minutes2 to 0 when it gets to 10
        }
    }
		//should be correct. hours1 and hours2 are what will be displayed. 
		new_hours = hours1*10 +hours2;
		new_minutes = minutes1*10 +minutes2;
		new_seconds = seconds1*10 + seconds2;
		
	
}
void Increment_6Digit_Timer(void){
		if (selected6Digit == 0) {
        timer_hours1++;
			
        if (timer_hours1 > 9) {
            timer_hours1 = 0; // Reset hours1 to 0 when it gets to 2
        }
    }
    else if (selected6Digit == 1) {
        timer_hours2++;
        if (timer_hours2 > 9) {
            timer_hours2 = 0; // Reset hours2 to 0 when it gets to 3 
        }
    }
    else if (selected6Digit == 2) {
        timer_minutes1++;
        if (timer_minutes1 > 5) {
            timer_minutes1 = 0; // Reset minutes1 to 0 when it gets to 6
        }
    }
    else if (selected6Digit == 3) {
        timer_minutes2++;
        if (timer_minutes2 > 9) {
            timer_minutes2 = 0; // Reset minutes2 to 0 when it gets to 10
        }
    }
		else if (selected6Digit == 4) {
        timer_seconds1++;
        if (timer_seconds1 > 5) {
            timer_seconds1 = 0; // Reset seconds1 to 0 when it gets to 6
        }
    }
		else if (selected6Digit == 5) {
        timer_seconds2++;
        if (timer_seconds2 > 9) {
            timer_seconds2 = 0; // Reset minutes2 to 0 when it gets to 10
        }
    }
		
	

	
}
void Increment_4Digit(void){ //alarm mode. 
	if (selected4Digit == 0) {
        if (hourMode == 0) { // 12-hour mode
            alarm_hours1++;
            if (alarm_hours1 > 1) {
                alarm_hours1 = 0; // Reset hours1 to 0 when it gets to 2
            }
        } else if (hourMode == 1) { // 24-hour mode
            alarm_hours1++;
            if (alarm_hours1 > 2) {
                alarm_hours1 = 0; // Reset hours1 to 0 when it gets to 3
            }
        }
    }
    else if (selected4Digit == 1) {
        alarm_hours2++;
        if (alarm_hours2 > 9) {
            alarm_hours2 = 0; // Reset hours2 to 0 when it gets to 3 
        }
    }
    else if (selected4Digit == 2) {
        alarm_minutes1++;
        if (alarm_minutes1 > 5) {
            alarm_minutes1 = 0; // Reset minutes1 to 0 when it gets to 6
        }
    }
    else if (selected4Digit == 3) {
        alarm_minutes2++;
        if (alarm_minutes2 > 9) {
            alarm_minutes2 = 0; // Reset minutes2 to 0 when it gets to 10
        }
    }
}
void Twelveto24(){
		if (new_hours == 12 && AMFlag == 0) {
        new_hours = 0; // 12 AM in 24-hour format is 0
    } else if (AMFlag == 1 && new_hours != 12) {
        new_hours = hours + 12; // PM hours are shifted by 12
    } else {
        new_hours = hours; // For 1 to 11 AM, and 12 PM, the hours remain the same
    }
    
}
void Twentyto12(){
	if (hours == 0) {
        new_hours = 12; // 0 in 24-hour format is 12 AM in 12-hour format
        AMFlag = 0; // Set AM flag
    } else if (hours >= 1 && hours <= 12) {
        new_hours = hours; // For 1 to 12, the hours remain the same
        AMFlag = 0; // Set AM flag
    } else {
        new_hours = hours - 12; // Convert 13 to 23 to 1 to 11 PM
        AMFlag = 1; // Set PM flag
    }

	
}
