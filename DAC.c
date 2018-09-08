// dac.c
// This software configures DAC output
// Runs on LM4F120 or TM4C123
// Program written by: Kunal Jain and Amrith Lotlikar
// Date Created: 3/6/17 
// Last Modified: 3/20/18 
// Hardware connections
// PB0, PB1, PB2, PB3

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "DAC.h"


const unsigned char Flute[64] = {  	 // A flute's sine wave represented in 4 bits
  6,7,9,9,10,11,12,13,13,14,15,	
  15,15,15,15,14,13,13,12,11,10,9,	
  8,7,7,6,6,5,5,5,5,4,	
  4,4,4,4,4,3,3,3,3,3,	
  2,2,1,1,1,1,0,0,0,0,	
  0,0,1,1,1,2,2,3,3,4,4,5	
};

// **************DAC_Init*********************
// Initialize 4-bit DAC, called once
// PB0, PB1, PB2, PB3
// Input: none
// Output: none
void DAC_Init(void){
	uint32_t delay = SYSCTL_RCGC2_R;
	SYSCTL_RCGCGPIO_R |= 0x02; 					// Port B 0-3 is initialized
	delay ++;
	delay ++;
	GPIO_PORTB_AMSEL_R &= 0xF0;
	GPIO_PORTB_AFSEL_R &= 0xF0;
	GPIO_PORTB_DIR_R |= 0x0F;
	GPIO_PORTB_DEN_R |= 0x0F;
}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Input=n is converted to n*3.3V/15
// Output: none
void DAC_Out(uint32_t data){
	//uint8_t B = GPIO_PORTB_DATA_R & 0xF0;					// Writes only to Port B 0-3
	//B |= data;
	GPIO_PORTB_DATA_R = data;
}
