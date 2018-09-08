// Buttons.c
// This software configures Button input
// Runs on LM4F120 or TM4C123
// Program written by: Kunal Jain and Amrith Lotlikar 
// Last Modified: 4/29/18 
// Hardware connections
// PB4, PB5, PB6

#include "tm4c123gh6pm.h"

// **************Buttons_Init*********************
// Initialize 3 game buttons, called once
// PB4, PB5, PB6
// Input: none
// Output: none
void Buttons_Init(void){
	volatile int delay = 0;
	SYSCTL_RCGCGPIO_R |= 0x02; 		// Port B 4-6 is initialized
	delay ++;
	delay ++;
	GPIO_PORTB_DIR_R &= ~0x70;
	GPIO_PORTB_DEN_R |= 0x70;
}
