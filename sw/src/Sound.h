#include <stdint.h>
//high
#define C6   597   // 2093 Hz
#define B6   633   // 1975.5 Hz
#define BF6  670   // 1864.7 Hz
#define A6   710   // 1760 Hz
#define AF6  752   // 1661.2 Hz
#define G6   797   // 1568 Hz
#define GF6  845   // 1480 Hz
#define F6   895   // 1396.9 Hz
#define E6   948   // 1318.5 Hz
#define EF6  1004   // 1244.5 Hz
#define D6   1064   // 1174.7 Hz
#define DF6  1127   // 1108.7 Hz
#define C5    1194   // 1046.5 Hz
#define B5    1265   // 987.8 Hz
#define BF5   1341   // 932.3 Hz
#define A5    1420   // 880 Hz
#define AF5   1505   // 830.6 Hz
#define G5    1594   // 784 Hz
#define GF5  1689   // 740 Hz
#define F5   1790   // 698.5 Hz
#define E5   1896   // 659.3 Hz
#define EF5  2009   // 622.3 Hz
#define D5	2128   // 587.3 Hz
#define DF5  2255   // 554.4 Hz
#define C4  2389   // 523.3 Hz //first
#define B4  2531   // 493.9 Hz
#define BF4 2681   // 466.2 Hz
#define A4  2841   // 440 Hz
#define AF4 3010   // 415.3 Hz
#define G4  3189   // 392 Hz
#define GF4 3378   // 370 Hz
#define F4  3579   // 349.2 Hz
#define E4  3792   // 329.6 Hz
#define EF4 4018   // 311.1 Hz
#define D4  4257   // 293.7 Hz
#define DF4 4510   // 277.2 Hz
#define C3  4778   // 261.6 Hz middle c
#define B3  5062   // 246.9 Hz
#define BF3 5363   // 233.1 Hz
#define A3  5682   // 220 Hz
#define AF3 6020   // 207.7 Hz
#define G3  6378   // 196 Hz
#define GF3 6757   // 185 Hz
#define F3  7159   // 174.6 Hz
#define E3  7584   // 164.8 Hz
#define EF3 8035   // 155.6 Hz
#define D3  8513   // 146.8 Hz
#define DF3 9019   // 138.6 Hz
#define C2  9556   // 130.8 Hz
#define rest 0xFFFFFFFF // period too large to create sound!!! outside of human earing range

//beat duration (seconds) = 60/BPM -> duration of one beat
//ReloadValue = 80MHz * beatduration -> this depends on whether its a quarter, whole, half, eigth, etc
#define bpm80 2500000 //interrupt period for 80 bpm -- 24 interrupts per beat -- formula in lab 10 onenote 1/24th of a beat
#define bpm110 1818182 
#define bpm158 1265823
#define bpm100 2000000
#define bpm132 1515151 

#define space 1
#define whole 96-space
#define half 48-space
#define dottedhalf 72-space
#define quarter 24-space		//24 pixels in quarter note
#define eighth 12 - space
#define sixteenth 6-space
#define triplet 8-space
#define twotriplet 16-space
#define quartertriplet 32-space
#define wholeeighth 108-space
#define dottedeighth 18-space
#define dottedhalfeighth 84-space
#define dottedquarter 36-space
#define eighttriplet 4-space


#define lamb 0
#define rainy 1
#define driftveil 4



struct note {
	//uint8_t alterflag;		//should button handler play alternate note (row) also dont need this. 
	uint8_t idx; //0,1,2,3,4,5 - index to song's pitches array (column)
	uint32_t length; // units of 12.5 nanoseconds
	//uint32_t pixels;//dont need this...
};
typedef struct note note_t;
struct song {
	uint32_t pitches[30]; // array of pitches necessary for song
	uint32_t tempo;		//tempo of the song
	uint32_t N;			//number of notes
	note_t notes[160]; //notes in song
};
typedef struct song song_t;
extern song_t songs[5];

void PlaySound(int);