// Sound.c
// Runs on any TM4C123
// Sound assets based off the original Space Invaders
// Import these constants into your SpaceInvaders.c for sounds!
// Jonathan Valvano
// 11/15/2021
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Sound.h"
#include "../inc/DAC.h"

const uint16_t wave_12bit[64] = {
    2048, 2447, 2831, 3185, 3495, 3750, 3939, 4056, 4095, 4056, 3939, 3750,
    3495, 3185, 2831, 2447, 2048, 1648, 1264, 910, 600, 345, 156, 39, 0, 39,
    156, 345, 600, 910, 1264, 1648, 2047, 2447, 2832, 3186, 3495, 3750, 3939,
    4056, 4095, 4056, 3939, 3750, 3495, 3185, 2831, 2447, 2048, 1648, 1264,
    910, 600, 345, 156, 39, 0, 39, 156, 345, 600, 910, 1264, 1648
};
song_t songs[5] = {
	//mary had a little lamb
	{{C3,G4,A4,F4,E4,D4,AF4,AF4,rest}, bpm132, 14,
	{{0,bpm132*quarter}, {8,bpm132*space}, {0,bpm132*quarter}, {8,bpm132*space}, {1,bpm132*quarter}, {8,bpm132*space}, {1,bpm132*quarter}, {8,bpm132*space}, {2,bpm132*quarter}, {8,bpm132*space}, {2,bpm132*quarter},
	{1, bpm132*half}, {8,bpm132*space},{3, bpm132*quarter}, {8,bpm132*space},{3, bpm132*quarter}, {8,bpm132*space},{4,bpm132*quarter}, {8,bpm132*space},{4,bpm132*quarter}, {8,bpm132*space},{5 ,bpm132*quarter},
	{8,bpm132*space}, {5 ,bpm132*quarter}, {8,bpm132*space},{0,bpm132*half}}},
	//rainy
	{{BF5, C5, G5, AF5, F5, BF4,
      EF5, C5, G5, AF5, F5, BF4, rest}, bpm110, 92, {}
//	{{0,0,43636362, quarter}, {0,0,43636362, quarter},    {0,0,43636362, quarter}, {0,1,29090909, twotriplet}, {0,2,14545454, triplet},
//     {0,13,87272727, half},    {0,13,29090909, twotriplet}, {0,2,14545454, triplet}, {0,3,29090909, twotriplet}, {0,0,14545454,triplet},
//     {0,13,43636362, quarter}, {0,0,43636362, quarter},    {0,0,43636362, quarter}, {0,1,29090909, twotriplet}, {0,2,14545454, triplet},
//     {0,13,87272727, half},    {0,13,29090909, twotriplet}, {0,2,14545454, triplet}, {0,4,29090909, twotriplet}, {0,2,14545454, triplet},
//     {0,13,174545454, whole},
//     {0,0,29090909, twotriplet}, {0,2,14545454, triplet}, {0,3,29090909, twotriplet}, {0,0,58181818, quartertriplet}, {0,5,29090909, twotriplet}, {0,3,14545454, triplet},
//     {0,13,174545454, whole},
//     {0,2,29090909, twotriplet}, {1,0,14545454, triplet}, {0,4,29090909, twotriplet}, {0,2,58181818, quartertriplet}, {0,5,29090909, twotriplet}, {0,4,14545454, triplet},
//     {0,13,174545454, whole},
//     {0,0,29090909, twotriplet}, {0,2,14545454, triplet}, {0,3,29090909, twotriplet}, {0,0,58181818, quartertriplet}, {0,5,29090909, twotriplet}, {0,1,58181818, quartertriplet},
//     {0,0,29090909, twotriplet}, {0,3,58181818, quartertriplet}, {1,0,29090909, twotriplet}, {0,2,14545454, triplet},
//     {0,13,174545454, whole},
//     {0,0,43636362, quarter}, {0,0,43636362, quarter},    {0,0,43636362, quarter}, {0,1,29090909, twotriplet}, {0,2,14545454, triplet},
//     {0,13,87272727, half},    {0,13,29090909, twotriplet}, {0,2,14545454, triplet}, {0,3,29090909, twotriplet}, {0,0,14545454,triplet},
//     {0,13,43636362, quarter}, {0,0,43636362, quarter},    {0,0,43636362, quarter}, {0,1,29090909, twotriplet}, {0,2,14545454, triplet},
//     {0,13,87272727, half},    {0,13,29090909, twotriplet}, {0,2,14545454, triplet}, {0,4,29090909, twotriplet}, {0,2,14545454, triplet},
//     {0,13,174545454, whole},
//     {0,0,29090909, twotriplet}, {0,2,14545454, triplet}, {0,3,29090909, twotriplet}, {0,0,58181818, quartertriplet}, {0,5,29090909, twotriplet}, {0,3,14545454, triplet},
//     {0,13,174545454, whole},
//     {0,2,29090909, twotriplet}, {1,0,14545454, triplet}, {0,4,29090909, twotriplet}, {0,2,58181818, quartertriplet}, {0,5,29090909, twotriplet}, {0,4,14545454, triplet},
//     {0,13,174545454, whole},
//     {0,0,29090909, twotriplet}, {0,2,14545454, triplet}, {0,3,29090909, twotriplet}, {0,0,58181818, quartertriplet}, {0,5,29090909, twotriplet}, {0,1,58181818, quartertriplet},
//     {0,0,29090909, twotriplet}, {0,3,58181818, quartertriplet}, {1,0,29090909, twotriplet}, {0,2,14545454, triplet},
//     {0,13,174545454, whole},
//    }
},
	//nooks cranny
    {{AF4, BF4, B4, F5, EF5, DF5,
      GF3, DF4, DF3, GF4, C4, E5, rest}, bpm158, 124,{}
//    {{1,0,30379746, quarter}, {1,1,15189873, eighth}, {1,1,15189873, eighth}, {1,1,30379746, quarter}, {1,2,30379746, quarter},
//     {1,0,30379746, quarter}, {1,1,7594937, sixteenth}, {1,1,7594937, sixteenth}, {1,1,7594937, sixteenth}, {1,1,7594937, sixteenth}, {1,1,30379746, quarter}, {1,2,30379746, quarter},
//     {1,0,30379746, quarter}, {1,1,15189873, eighth}, {1,1,15189873, eighth}, {1,1,30379746, quarter}, {1,2,30379746, quarter},
//     {1,0,30379746, quarter}, {1,1,7594937, sixteenth}, {1,1,7594937, sixteenth}, {1,1,7594937, sixteenth}, {1,1,7594937, sixteenth}, {1,1,30379746, quarter}, {1,2,30379746, quarter},
//     {0,13,30379746, quarter},{0,0,15189873, eighth}, {0,1,15189873,eighth}, {0,2,30379746, quarter}, {0,0,30379746, quarter},
//     {0,3,60759494, half}, {0,4,60759494, half},
//     {0,13,30379746, quarter},{0,5,15189873, eighth}, {0,4,15189873,eighth}, {1,5,30379746, quarter}, {0,5,30379746, quarter},
//     {0,1,60759494, half}, {0,0,60759494, half},
//     {1,0,30379746, quarter}, {1,1,15189873, eighth}, {1,1,15189873, eighth}, {1,1,30379746, quarter}, {1,2,30379746, quarter},
//     {1,0,30379746, quarter}, {1,1,7594937, sixteenth}, {1,1,7594937, sixteenth}, {1,1,7594937, sixteenth}, {1,1,7594937, sixteenth}, {1,1,30379746, quarter}, {1,2,30379746, quarter},
//     {0,13,30379746, quarter},{0,0,15189873, eighth}, {0,1,15189873,eighth}, {0,2,30379746, quarter}, {0,0,30379746, quarter},
//     {0,3,60759494, half}, {0,4,30379746, quarter}, {0,5,30379746, quarter},
//     {1,4,60759494, half}, {0,1,30379746, quarter}, {0,0,30379746, quarter},
//     {1,3,121518987, whole},
//     {1,0,30379746, quarter}, {1,1,15189873, eighth}, {1,1,15189873, eighth}, {1,1,30379746, quarter}, {1,2,30379746, quarter},
//     {1,0,30379746, quarter}, {1,1,7594937, sixteenth}, {1,1,7594937, sixteenth}, {1,1,7594937, sixteenth}, {1,1,7594937, sixteenth}, {1,1,30379746, quarter}, {1,2,30379746, quarter},
//     {1,0,30379746, quarter}, {1,1,15189873, eighth}, {1,1,15189873, eighth}, {1,1,30379746, quarter}, {1,2,30379746, quarter},
//     {1,0,30379746, quarter}, {1,1,7594937, sixteenth}, {1,1,7594937, sixteenth}, {1,1,7594937, sixteenth}, {1,1,7594937, sixteenth}, {1,1,30379746, quarter}, {1,2,30379746, quarter},
//     {0,13,30379746, quarter},{0,0,15189873, eighth}, {0,1,15189873,eighth}, {0,2,30379746, quarter}, {0,0,30379746, quarter},
//     {0,3,60759494, half}, {0,4,60759494, half},
//     {0,13,30379746, quarter},{0,5,15189873, eighth}, {0,4,15189873,eighth}, {1,5,30379746, quarter}, {0,5,30379746, quarter},
//     {0,1,60759494, half}, {0,0,60759494, half},
//     {1,0,30379746, quarter}, {1,1,15189873, eighth}, {1,1,15189873, eighth}, {1,1,30379746, quarter}, {1,2,30379746, quarter},
//     {1,0,30379746, quarter}, {1,1,7594937, sixteenth}, {1,1,7594937, sixteenth}, {1,1,7594937, sixteenth}, {1,1,7594937, sixteenth}, {1,1,30379746, quarter}, {1,2,30379746, quarter},
//     {0,13,30379746, quarter},{0,0,15189873, eighth}, {0,1,15189873,eighth}, {0,2,30379746, quarter}, {0,0,30379746, quarter},
//     {0,3,60759494, half}, {0,4,30379746, quarter}, {0,5,30379746, quarter},
//     {1,4,60759494, half}, {0,1,30379746, quarter}, {0,0,30379746, quarter},
//     {1,3,121518987, whole}, 
    
	},
		//new horizons
    {{A4, F4, D5, C4, A5, G4, 
      D4, E5, F5, AF4, B4, DF4,rest}, bpm100, 162,{}
//     {{0,13,24000000, eighth}, {0,0,24000000, eighth}, {0,1,24000000,eighth}, {0,2,24000000,eighth}, {0,3,48000000, quarter}, {0,0,24000000, eighth}, {0,1,48000000, quarter},
//      {1,0,24000000, eighth}, {0,5,144000000, dottedhalf},
//      {0,1,24000000, eighth}, {0,5,24000000, eighth}, {0,0,24000000, eighth}, {0,2,48000000, quarter}, {1,1,24000000, eighth}, {1,2,24000000, eighth}, {0,3,216000000, wholeeighth},
//      {0,13,24000000, eighth}, {0,0,24000000, eighth}, {0,1,24000000,eighth}, {0,2,24000000,eighth}, {0,3,48000000, quarter}, {0,0,24000000, eighth}, {0,1,48000000, quarter},
//      {1,0,24000000, eighth}, {0,5,144000000, dottedhalf},
//      {0,1,24000000, eighth}, {0,5,24000000, eighth}, {0,0,24000000, eighth}, {0,2,48000000, quarter}, {1,1,24000000, eighth}, {1,2,24000000, eighth}, {0,3,216000000, wholeeighth},
//      {0,13,24000000, eighth}, {0,0,24000000, eighth}, {0,1,24000000,eighth}, {0,2,24000000,eighth}, {0,3,24000000,eighth}, {0,0,24000000,eighth}, {1,0,36000000, dottedeighth},
//      {0,1, 12000000, sixteenth}, {0,5, 168000000, dottedhalfeighth},
//      {0,13,48000000, quarter}, {0,0,48000000, quarter}, {0,1,48000000, quarter}, {0,2,48000000, quarter},
//      {0,0,72000000, dottedquarter}, {0,1,24000000, eighth}, {0,1, 192000000, whole},
//      {0,13,24000000, eighth}, {1,2,24000000, eighth}, {1,1,24000000, eighth}, {0,2,24000000, eighth},
//      {0,3,72000000, dottedquarter}, {0,1,24000000, eighth}, {0,1, 192000000, whole},
//      {0,13,24000000, eighth}, {1,2,24000000, eighth}, {1,1,24000000, eighth}, {0,2,24000000, eighth},
//      {1,1,72000000, dottedquarter}, {0,0,24000000, eighth}, {0,0, 192000000, whole},
//      {1,3,24000000, eighth},{0,0,24000000, eighth},{1,4,24000000, eighth}, {1,5,48000000, quarter},
//      {1,1,24000000, eighth}, {0,4,96000000,half}, {0,13,48000000, quarter},
//      {0,13,24000000, eighth}, {0,0,24000000, eighth}, {0,1,24000000,eighth}, {0,2,24000000,eighth}, {0,3,48000000, quarter}, {0,0,24000000, eighth}, {0,1,48000000, quarter},
//      {1,0,24000000, eighth}, {0,5,144000000, dottedhalf},
//      {0,1,24000000, eighth}, {0,5,24000000, eighth}, {0,0,24000000, eighth}, {0,2,48000000, quarter}, {1,1,24000000, eighth}, {1,2,24000000, eighth}, {0,3,216000000, wholeeighth},
//      {0,13,24000000, eighth}, {0,0,24000000, eighth}, {0,1,24000000,eighth}, {0,2,24000000,eighth}, {0,3,48000000, quarter}, {0,0,24000000, eighth}, {0,1,48000000, quarter},
//      {1,0,24000000, eighth}, {0,5,144000000, dottedhalf},
//      {0,1,24000000, eighth}, {0,5,24000000, eighth}, {0,0,24000000, eighth}, {0,2,48000000, quarter}, {1,1,24000000, eighth}, {1,2,24000000, eighth}, {0,3,216000000, wholeeighth},
//      {0,13,24000000, eighth}, {0,0,24000000, eighth}, {0,1,24000000,eighth}, {0,2,24000000,eighth}, {0,3,24000000,eighth}, {0,0,24000000,eighth}, {1,0,36000000, dottedeighth},
//      {0,1, 12000000, sixteenth}, {0,5, 168000000, dottedhalfeighth},
//      {0,13,48000000, quarter}, {0,0,48000000, quarter}, {0,1,48000000, quarter}, {0,2,48000000, quarter},
//      {0,0,72000000, dottedquarter}, {0,1,24000000, eighth}, {0,1, 192000000, whole},
//      {0,13,24000000, eighth}, {1,2,24000000, eighth}, {1,1,24000000, eighth}, {0,2,24000000, eighth},
//      {0,3,72000000, dottedquarter}, {0,1,24000000, eighth}, {0,1, 192000000, whole},
//      {0,13,24000000, eighth}, {1,2,24000000, eighth}, {1,1,24000000, eighth}, {0,2,24000000, eighth},
//      {1,1,72000000, dottedquarter}, {0,0,24000000, eighth}, {0,0, 192000000, whole},
//      {1,3,24000000, eighth},{0,0,24000000, eighth},{1,4,24000000, eighth}, {1,5,48000000, quarter},
//      {1,1,24000000, eighth}, {0,4,96000000,half}, {0,13,48000000, quarter},
//      {0,13,24000000, eighth}, {0,0,24000000, eighth}, {0,1,24000000,eighth}, {0,2,24000000,eighth}, {0,3,24000000,eighth}, {0,0,24000000,eighth}, {1,0,192000000, whole},
//      {0,5,24000000, eighth},
//      {0,1,192000000, whole}
//      }
      },
		//driftveil city
	{{EF3,BF3,A3,AF3,GF3,DF3,EF4, DF5, EF5, BF4, GF5, F5, AF4, AF5, BF5, F3,C3,DF4,A3,BF3, rest}, bpm132, 100,
	{{0,bpm132*eighth}, {20,bpm132*space}, {1,bpm132*eighth}, {20,bpm132*space}, {2,bpm132*eighth}, {20,bpm132*space}, {3,bpm132*eighttriplet}, {20,bpm132*space}, {2,bpm132*eighttriplet}, {20,bpm132*space}, {3,bpm132*eighttriplet}, {20,bpm132*space}, {4, bpm132*eighth}, {20,bpm132*space}, {5, bpm132*eighth}, {20,bpm132*space}, {0, bpm132*sixteenth}, {20,bpm132*space}, {0, bpm132*sixteenth}, {20,bpm132*space}, {17, bpm132*eighth}, {20,bpm132*space}, {6, bpm132*eighth}, {20,bpm132*space}, {6, bpm132*eighth}, {20,bpm132*space}, {6, bpm132*eighth}, {20,bpm132*space}, //beginning
	{7, bpm132*eighth}, {20,bpm132*space}, {8, bpm132*sixteenth}, {20,bpm132*space}, {8, bpm132*sixteenth}, {20,bpm132*space}, {9, bpm132*eighth}, {20,bpm132*space}, {7, bpm132*eighth}, {20,bpm132*space},{9, bpm100*eighth},{20,bpm132*space}, //measure 2
	{8, bpm132*dottedquarter}, {20,bpm132*space}, {10, bpm132*eighth}, {20,bpm132*space}, {11, bpm132*eighth},{20,bpm132*space}, {10, bpm132*eighth}, {20,bpm132*space}, {9, bpm132*eighth}, {20,bpm132*space}, {7, bpm132*eighth}, {20,bpm132*space}, //measure 3
	{8, bpm132*quarter},	{20,bpm132*space},{7, bpm132*eighth}, {20,bpm132*space},{8, bpm132*eighth}, {20,bpm132*space},{9, bpm132*dottedeighth}, {20,bpm132*space},{12, bpm132*sixteenth}, {20,bpm132*space},{9, bpm132*quarter},{20,bpm132*space}, //measure 4
	{8, bpm132*dottedquarter}, {20,bpm132*space}, {10, bpm132*eighth}, {20,bpm132*space}, {11, bpm132*eighth},{20,bpm132*space}, {10, bpm132*eighth}, {20,bpm132*space}, {13, bpm132*eighth}, {20,bpm132*space}, {14, bpm132*eighth}, {20,bpm132*space}, //measure 5
	{8, bpm132*quarter},	{20,bpm132*space},{7, bpm132*eighth}, {20,bpm132*space},{8, bpm132*eighth}, {20,bpm132*space},{11, bpm132*eighth}, {20,bpm132*space},{10, bpm132*sixteenth}, {20,bpm132*space},{11, bpm132*sixteenth},{20,bpm132*space}, {8, bpm132*quarter},{20,bpm132*space}, //measure 6
 	{13, bpm132*quarter}, {20,bpm132*space}, {13, bpm132*eighth},{20,bpm132*space}, {14, bpm132*eighth}, {20,bpm132*space},{10, bpm132*quarter},{20,bpm132*space}, {8, bpm132*eighth},{20,bpm132*space}, {10, bpm132*eighth},{20,bpm132*space},//measure 7
	{13, bpm132*dottedeighth}, {20,bpm132*space},{8, bpm132*sixteenth},{20,bpm132*space}, {13, bpm132*eighth},{20,bpm132*space}, {14, bpm132*eighth}, {20,bpm132*space},{10, bpm132*quarter}, {20,bpm132*space},{8, bpm132*eighth},{20,bpm132*space},{10, bpm132*eighth},{20,bpm132*space} //measure 8
	}}
};

void PlaySound (int pntr){
		dac_output(wave_12bit[pntr]);
}
