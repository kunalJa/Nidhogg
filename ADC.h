// ADC.h
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Student names: Kunal Jain
// Last modification date: May 1, 2018

#ifndef __ADC_H__ // do not include more than once
#define __ADC_H__

//-----------ADC_Init-----------
// ADC initialization function 
// Input: none
// Output: none
void ADC_Init(void);

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC_In(void);

//------------ADC_In2------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC_In2(void);

//------------Convert-------------
// Converts the approximate guessing into a linearized function
// Input: ADC_IN
// Output: (21789*input)/50000 + 23
uint32_t Convert(uint32_t input);

#endif // __ADC_H__
