// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Last Modified: 3/28/2018 
// Student names: Kunal Jain and Amrith Lotlikar
// Last modification date: May 1, 2018

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ADC.h"

// **************ADC_Init*********************
// ADC initialization function 
// Input: none
// Output: none
// measures from PD2, analog channel 5 and PE4, analog channel 2
void ADC_Init(void){ 
	//PortF_Init();
	volatile uint32_t delay = 0;
	// Joystick L/R: PD2
	SYSCTL_RCGCGPIO_R |= 0x08;      // 1) ENABLE PORT D
	delay ++;
	delay ++;
	delay ++;
	GPIO_PORTD_DIR_R &= ~0x04;      // 2) make PD2 input
  GPIO_PORTD_AFSEL_R |= 0x04;     // 3) enable alternate function on PD2
  GPIO_PORTD_DEN_R &= ~0x04;      // 4) disable digital I/O on PD2
  GPIO_PORTD_AMSEL_R |= 0x04;     // 5) enable analog functionality on PD2
	
	//GPIO_PORTF_DATA_R ^= 0x04;					// PF1 - PF3 are heartbeats
	SYSCTL_RCGCADC_R |= 0x00000001; // 6) activate ADC0
	//GPIO_PORTF_DATA_R ^= 0x04;					// PF1 - PF3 are heartbeatsd
	for (int i = 0; i < 10000; i++){
		delay ++;
	}
	delay --;
	ADC0_PC_R &= ~0x0F;							// 8) clear max sample rate field
  ADC0_PC_R |= 0x01;              // 9) configure for 125K samples/sec
	ADC0_SSPRI_R = 0x0123;          // 10) Sequencer 3 is highest priority
	ADC0_ACTSS_R &= ~0x08;
	ADC0_EMUX_R &= ~0xF000;
	ADC0_SSMUX3_R &= ~0xFF;
	ADC0_SSMUX3_R |= 0x05; 					// Channel 5
	ADC0_SSCTL3_R = 0x06;
	ADC0_IM_R &= ~0x08;
	ADC0_ACTSS_R |= 0x08;
	
	// Joystick U/D: PE4
  SYSCTL_RCGCGPIO_R |= 0x10;	 		// 1) activate clock for Port E
  delay ++;      									// 2) allow time for clock to stabilize
  delay ++;
	delay ++;
  GPIO_PORTE_DIR_R &= ~0x10;      // 3) make PE4 input
  GPIO_PORTE_AFSEL_R |= 0x10;     // 4) enable alternate function on PE4
  GPIO_PORTE_DEN_R &= ~0x10;      // 5) disable digital I/O on PE4
  GPIO_PORTE_AMSEL_R |= 0x10;     // 6) enable analog functionality on PE4

	//GPIO_PORTF_DATA_R ^= 0x04;					// PF1 - PF3 are heartbeats
  ADC0_ACTSS_R &= ~0x0004;        // 7) disable sample sequencer 2
  ADC0_EMUX_R &= ~0x0F00;         // 8) seq2 is software trigger
  ADC0_SSMUX2_R = 0x0089;         // 9) set channels for SS2
  ADC0_SSCTL2_R = 0x0060;         // 10) no TS0 D0 IE0 END0 TS1 D1, yes IE1 END1
  ADC0_IM_R &= ~0x0004;           // 11) disable SS2 interrupts
  ADC0_ACTSS_R |= 0x0004;         // 12) enable sample sequencer 2
}

//****************ADC_In*********************
// Busy-wait Analog to digital conversion for joystick L/R
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 5
uint32_t ADC_In(void){  
	ADC0_PSSI_R = 0x0008;            								// 1) initiate SS3
  while((ADC0_RIS_R&0x08)==0){};  					  		// 2) wait for conversion done
  uint32_t result = ADC0_SSFIFO3_R & 0xFFF;   		// 3) read result
  ADC0_ISC_R = 0x0008;            						 		// 4) acknowledge completion
  return result;
}

//****************ADC_In2*********************
// Busy-wait Analog to digital conversion for joystick U/D
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PE4, analog channel 2
uint32_t ADC_In2(){ 
	uint32_t result;
	ADC0_PSSI_R = 0x0004;           							 // 1) initiate SS2
  while((ADC0_RIS_R&0x04)==0){};  							 // 2) wait for conversion done
	result = ADC0_SSFIFO2_R&0xFFF;  			 // 3) read result
  ADC0_ISC_R = 0x0004;            							 // 4) acknowledge completion
  return result;
}


//------------Convert-------------
// Converts the approximate guessing into a linearized function
// Input: ADC_IN
// Output: (21789*input)/50000 + 23
uint32_t Convert(uint32_t input){
  return (21789*input)/50000 + 23;
}
