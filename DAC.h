// Dac.h
// Runs on LM4F120/TM4C123
// Initializes and outputs to the 4 bit DAC for sound
// Kunal Jain and Amrith Lotlikar
// May 1, 2018

#ifndef __DAC_H__ // do not include more than once
#define __DAC_H__

// **************DAC_Init*********************
// Initialize DAC port-configuration
// Input: none
// Output: none
void DAC_Init(void);
	
// **************DAC_Out*********************
// Write to Port B i.e the 4 bit DAC
// Input: uint32_t output voltage
// Output: none
void DAC_Out(uint32_t data);

#endif // __DAC_H__
