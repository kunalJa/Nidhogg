// SysTick.c
// This software configures SysTick for Periodic timed interrupt
// Runs on LM4F120 or TM4C123
// Program written by: Kunal Jain and Amrith Lotlikar
// Date Created: 3/6/17 
// Last Modified: 3/20/18 

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "DAC.h"

#define SYSINIT_VAL 123456

extern unsigned char Flute[];
extern uint32_t Flute_ind1;
extern uint32_t Flute_ind2;

// **************SysTick_Init*********************
// Initialize SysTick periodic interrupts
// Input: none
// Output: none
void SysTick_Init(void){
	NVIC_ST_CTRL_R = 0;         	// disable SysTick during setup
  NVIC_ST_RELOAD_R = SYSINIT_VAL;		// reload value 24 frames per second
  NVIC_ST_CURRENT_R = 0;        // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // priority 2
  NVIC_ST_CTRL_R |= 0x07;  		  // enable SysTick with core clock and interrupts
}

// Interrupt service routine
// Executed 24 times a second, allowing the display to be re-printed 24 times a second
void SysTick_Handler(void){ 
	if( NVIC_ST_RELOAD_R != SYSINIT_VAL){
		//uint32_t outval = (Flute[Flute_ind1] + Flute[Flute_ind2]) / 2;
		uint32_t outval = Flute[Flute_ind1];
		DAC_Out(outval);
		Flute_ind1 = (Flute_ind1 + 1) % 64;
	}
}

// SysTick is being used for Sound functionality
