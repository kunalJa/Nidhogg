// Sound.c
// Enables for the creation of music
// Kunal Jain and Amrith Lotlikar
// May 1, 2018
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "Sound.h"
#include "DAC.h"
#include "SysTick.h"
#include "Timer1.h"
#include "Timer2.h"
#include "Timer3.h"

typedef struct song{
	uint32_t note;
	uint32_t beat_val;
} song_t;

// *************************** Songs/Sounds ***************************
// Main track
const song_t XIV[] = {
	{E, EIGHTH}, {0, 200}, {F, EIGHTH}, {0, 200}, {G, EIGHTH}, {0, 200}, {F, EIGHTH}, {0, 200}, {E, EIGHTH}, {0, 200}, {F, EIGHTH}, {0, 200}, {G, EIGHTH}, 
  {0, 200}, {F, EIGHTH},	{0, 200}, {B, EIGHTH}, {0, 200}, {A, EIGHTH}, {0, 200}, {G, EIGHTH}, {0, 200}, {A, EIGHTH}, {0, 200}, {G, EIGHTH}, {0, 200}, 
	{F, EIGHTH}, {0, 200}, {EF, EIGHTH}, {0, 200}, {E, EIGHTH}
};
uint32_t XIV_size = 31;

// Main track backing
const song_t XIV_2[] = {
	{B0, QUARTER + 400}, {0, HALF}, {0, QUARTER}, {0, WHOLE + 13*200}
};
uint32_t XIV_2_size = 4;
// ********************************************************************

uint32_t Flute_ind1;
uint32_t Flute_ind2;

void Sound_Init(void){
	Flute_ind1 = 0;	
	Flute_ind2 = 0;
	SysTick_Init();
	Timer1_Init();
	//Timer2_Init();
	//Timer3_Init();
	DAC_Init();
};

// **************Sound_Play*********************
// Start sound output, and set Systick interrupt period 
// Sound continues until Sound_Play called again
// This function returns right away and sound is produced using a periodic interrupt
// Input: interrupt period
//        Units of period are {12.5ns} (assuming 80 MHz clock) 
//        Maximum period to be determined by YOU
//        Minimum period to be determined by YOU
//        if period equals zero, disable sound output
// Output: none
void Sound_Play(uint32_t period){
	if (period == 0){
		NVIC_ST_CTRL_R = 0;         		// disable SysTick for no sound being played
		DAC_Out(0);
	}
	else{
		NVIC_ST_RELOAD_R = period - 1;	// reload value
		NVIC_ST_CTRL_R = 0x07;  				// enable SysTick with core clock and interrupts
	}
}

uint32_t XIV_ind= 0;
void Timer1A_Handler(void){
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
	TIMER1_TAILR_R = XIV[XIV_ind].beat_val;
	Sound_Play(XIV[XIV_ind].note);
	XIV_ind = (XIV_ind + 1) % 31;
	//GPIO_PORTF_DATA_R ^= 0x8;
}
