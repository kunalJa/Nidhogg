// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 4/29/2018 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2017

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2017

 Copyright 2017 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// ******* Hardware I/O connections*******************
// Joystick L/R connected to PD2
// Joystick U/D connected to PE4
// Jump button connected to PB4
// Lunge button connected to PB5
// High Swing button connected to PB6

// UART:
// Receive PC4
// Transmit PC5

// DAC:
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)

// LCD:
// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
//#include "Random.h"
#include "PLL.h"
#include "ADC.h"
#include "Timer0.h"
#include "images.h"
#include "UART.h"
#include "Sound.h"
#include "Buttons.h"

#define Orange 0x03FF
#define Yellow 0x07FF
#define Blue 0x0F800
#define Red 0x007F
#define Green 0x07E0
#define Pink 0xB17F
#define Purple 0x80CF
#define Gray 0xC6F8
#define White 0xFFFF
#define Black 0x0000
#define Brown 0x12CF

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

//uint8_t displayed = 0;

// RAM Declarations
extern unsigned short RestYellow[];
extern unsigned short RestYellow2[];
extern unsigned short RestBlue[];
extern unsigned short RestBlue2[];
extern unsigned short Sword[];
extern unsigned short SwordDown[];
extern unsigned short HisSword[];
extern unsigned short HisSwordDown[];
extern unsigned short PullUp[];
extern unsigned short HePullUp[];
extern unsigned short Lunge[];
extern unsigned short LungeLow[];
extern unsigned short HeLunge[];
extern unsigned short HeLungeLow[];
extern unsigned short BackArrow[];
extern unsigned short Nidhogg[];

// ROM Declarations
extern unsigned short WalkLegsClose[];
extern unsigned short WalkLegsOpen[];
extern unsigned short LungeUp[];
extern unsigned short HighLunge[];
extern unsigned short LowLunge[];
extern unsigned short UpSword[];
extern unsigned short Back[];
extern unsigned short Logo[];

// Charachter Colors + Background colors
uint16_t me_color = Yellow;
uint16_t enemy_color = Blue; 
uint16_t backgroundArr[] = {0x0000, 0x1111, 0x0101, 0x1010};

enum JoyStick {Right, Left, Up, Down, None} Joystick_Position;
int IO_Pressed = 0;
int JoyStick_sampler = 0;

int inOptions = 0;
int inMainMenu = 0;
int LobbyCreation = 0;

void IO_Wait(){
	if (JoyStick_sampler < 109){
		JoyStick_sampler ++;
		Joystick_Position = None;
		return;
	}
	JoyStick_sampler = 0;
	if ((GPIO_PORTB_DATA_R & 0x40) == 1){
		IO_Pressed = 1;
		Joystick_Position = None;
		return;
	}
	while((GPIO_PORTB_DATA_R & 0x40) == 0){
		int joyStick_LR = Convert(ADC_In());
		int joyStick_UD = Convert(ADC_In2());
		if (joyStick_LR > 1400){
			if(inOptions){
				ST7735_FillRect(52, 85, 65, 30, Black);
				ST7735_FillRect(0, 17, 15, 15, Black);
			}
			if(inMainMenu){
				ST7735_FillRect(37, 59, 90, 80, Brown);
			}
			Joystick_Position = Right;
			return;
		}
		else if (joyStick_LR < 400){
			if(inOptions){
				ST7735_FillRect(52, 85, 65, 30, Black);
				ST7735_FillRect(0, 17, 15, 15, Black);
			}
			if(inMainMenu){
				ST7735_FillRect(37, 59, 90, 80, Brown);
			}
			Joystick_Position = Left;
			return;
		}
		else if (joyStick_UD > 1400){
			if (inOptions){
				ST7735_FillRect(52, 85, 65, 30, Black);
				ST7735_FillRect(0, 17, 15, 15, Black);
			}
			Joystick_Position = Up;
			return;
		}
		else if (joyStick_UD < 400){
			Joystick_Position = Down;
			if(inOptions){
				ST7735_FillRect(52, 85, 65, 30, Black);
				ST7735_FillRect(0, 17, 15, 15, Black);
			}
			return;
		}
	}
	IO_Pressed = 1;
	return;
}


void wait_ms(uint32_t n){
	uint32_t volatile time;
  for (int i = 0; i < n; i++){
    time = 1598;  // 1msec, tuned at 80 MHz
    while(time){
	  	time--;
    }
  }
}

void Options(){
	inOptions = 1;
	inMainMenu = 0;
	ST7735_FillScreen(Black);
	MoveToRam(20, 16, Back, BackArrow);
	MoveToRam(24, 29, WalkLegsClose, RestYellow);
	ST7735_DrawBitmap(0, 16, BackArrow, 20, 16);
	ST7735_DrawBitmap(73, 40, RestYellow, 24, 29);
	ST7735_FillRect(33+20, 70, 14, 14, Brown);
	ST7735_FillRect(58+20, 70, 14, 14, Brown);
	ST7735_FillRect(83+20, 70, 14, 14, Brown);
	
	ST7735_FillRect(34+20, 71, 12, 12, Yellow); // yellow blue
	ST7735_DrawFastVLine(34+20, 71, 12,Blue);
	ST7735_DrawFastVLine(35+20, 71, 11,Blue);
	ST7735_DrawFastVLine(36+20, 71, 10,Blue);
	ST7735_DrawFastVLine(37+20, 71, 9, Blue);
	ST7735_DrawFastVLine(38+20, 71, 8, Blue);
	ST7735_DrawFastVLine(39+20, 71, 7, Blue);
	ST7735_DrawFastVLine(40+20, 71, 6, Blue);
	ST7735_DrawFastVLine(41+20, 71, 5, Blue);
	ST7735_DrawFastVLine(42+20, 71, 4, Blue);
	ST7735_DrawFastVLine(43+20, 71, 3, Blue);
	ST7735_DrawFastVLine(44+20, 71, 2, Blue);
	ST7735_DrawPixel(45+20, 71, Blue);
	
	ST7735_FillRect(59+20, 71, 12, 12, Red); // red green
	ST7735_DrawFastVLine(59+20, 71, 12,Green);
	ST7735_DrawFastVLine(60+20, 71, 11,Green);
	ST7735_DrawFastVLine(61+20, 71, 10,Green);
	ST7735_DrawFastVLine(62+20, 71, 9, Green);
	ST7735_DrawFastVLine(63+20, 71, 8, Green);
	ST7735_DrawFastVLine(64+20, 71, 7, Green);
	ST7735_DrawFastVLine(65+20, 71, 6, Green);
	ST7735_DrawFastVLine(66+20, 71, 5, Green);
	ST7735_DrawFastVLine(67+20, 71, 4, Green);
	ST7735_DrawFastVLine(68+20, 71, 3, Green);
	ST7735_DrawFastVLine(69+20, 71, 2, Green);
	ST7735_DrawPixel(70+20, 71, Green);
	
	ST7735_FillRect(84+20, 71, 12, 12, Pink); // pink purple
	ST7735_DrawFastVLine(84+20, 71, 12, Purple);
	ST7735_DrawFastVLine(85+20, 71, 11, Purple);
	ST7735_DrawFastVLine(86+20, 71, 10, Purple);
	ST7735_DrawFastVLine(87+20, 71, 9,  Purple);
	ST7735_DrawFastVLine(88+20, 71, 8,  Purple);
	ST7735_DrawFastVLine(89+20, 71, 7,  Purple);
	ST7735_DrawFastVLine(90+20, 71, 6,  Purple);
	ST7735_DrawFastVLine(91+20, 71, 5,  Purple);
	ST7735_DrawFastVLine(92+20, 71, 4,  Purple);
	ST7735_DrawFastVLine(93+20, 71, 3,  Purple);
	ST7735_DrawFastVLine(94+20, 71, 2,  Purple);
	ST7735_DrawPixel(95+20, 71, Purple);
	
	int cursor_pos = 0;
	while(1){
		if (cursor_pos == 0){
			ST7735_DrawFastHLine(58, 92, 5, White);
			ST7735_DrawFastHLine(59, 91, 3, White);
			ST7735_DrawFastHLine(60, 90, 1, White);
			Recolor(24, 29, WalkLegsClose, RestYellow, Orange, Yellow);
			ST7735_DrawBitmap(73, 40, RestYellow, 24, 29);
			IO_Wait();
			if (IO_Pressed){
				me_color = Yellow;
				enemy_color = Blue;
				IO_Pressed = 0;
				ST7735_DrawFastHLine(58, 92, 5, Green);
				ST7735_DrawFastHLine(59, 91, 3, Green);
				ST7735_DrawFastHLine(60, 90, 1, Green);
				wait_ms(900);
				ST7735_DrawFastHLine(58, 92, 5, White);
				ST7735_DrawFastHLine(59, 91, 3, White);
				ST7735_DrawFastHLine(60, 90, 1, White);
			}
			else{
				if (Joystick_Position == Right){
					cursor_pos ++;
				}
				else if (Joystick_Position == Left){
					cursor_pos = 3;
				}
				else{
					continue;
				}
			}
		}
		else if (cursor_pos == 1){
			ST7735_DrawFastHLine(83, 92, 5, White);
			ST7735_DrawFastHLine(84, 91, 3, White);
			ST7735_DrawFastHLine(85, 90, 1, White);
			Recolor(24, 29, WalkLegsClose, RestYellow, Orange, Red);
			ST7735_DrawBitmap(73, 40, RestYellow, 24, 29);
			IO_Wait();
			if (IO_Pressed){
				me_color = Red;
				enemy_color = Green;
				IO_Pressed = 0;
				ST7735_DrawFastHLine(83, 92, 5, Green);
				ST7735_DrawFastHLine(84, 91, 3, Green);
				ST7735_DrawFastHLine(85, 90, 1, Green);
				wait_ms(900);
				ST7735_DrawFastHLine(83, 92, 5, White);
				ST7735_DrawFastHLine(84, 91, 3, White);
				ST7735_DrawFastHLine(85, 90, 1, White);
			}
			else{
				if (Joystick_Position == Right){
					cursor_pos ++;
				}
				else if (Joystick_Position == Left){
					cursor_pos --;
				}
				else{
					continue;
				}
			}
		}
		else if (cursor_pos == 2){
			ST7735_DrawFastHLine(108, 92, 5, White);
			ST7735_DrawFastHLine(109, 91, 3, White);
			ST7735_DrawFastHLine(110, 90, 1, White);
			Recolor(24, 29, WalkLegsClose, RestYellow, Orange, Pink);
			ST7735_DrawBitmap(73, 40, RestYellow, 24, 29);
			IO_Wait();
			if (IO_Pressed){
				me_color = Pink;
				enemy_color = Purple;
				IO_Pressed = 0;
				ST7735_DrawFastHLine(108, 92, 5, Green);
				ST7735_DrawFastHLine(109, 91, 3, Green);
				ST7735_DrawFastHLine(110, 90, 1, Green);
				wait_ms(900);
				ST7735_DrawFastHLine(108, 92, 5, White);
				ST7735_DrawFastHLine(109, 91, 3, White);
				ST7735_DrawFastHLine(110, 90, 1, White);
			}
			else{
				if (Joystick_Position == Right){
					cursor_pos ++;
				}
				else if (Joystick_Position == Left){
					cursor_pos --;
				}
				else{
					continue;
				}
			}
		}
		else if (cursor_pos == 3){
			ST7735_DrawFastHLine(7, 25, 5, White);
			ST7735_DrawFastHLine(8, 24, 3, White);
			ST7735_DrawFastHLine(9, 23, 1, White);
			wait_ms(9); // Inorder to not call the I0_Wait too quickly
			IO_Wait();
			if (IO_Pressed){
				IO_Pressed = 0;
				ST7735_DrawFastHLine(7, 25, 5, Green);
				ST7735_DrawFastHLine(8, 24, 3, Green);
				ST7735_DrawFastHLine(9, 23, 1, Green);
				wait_ms(900);
				ST7735_DrawFastHLine(7, 25, 5, White);
				ST7735_DrawFastHLine(8, 24, 3, White);
				ST7735_DrawFastHLine(9, 23, 1, White);
				break;
			}
			else{
				if (Joystick_Position == Right){
					cursor_pos = 0;
				}
				else if (Joystick_Position == Left){
					cursor_pos --;
				}
				else{
					continue;
				}
			}
		}
	}
	inOptions = 0;
}


const char start[] = "start";
const char options[] = "options";
const char credits[] = "credits";

char toUpperCase(char s){
	if (s > 96 && s < 123){
		return s - 32;
	}
	return s;
}

char toLowerCase(char s){
	if (s > 64 && s < 91){
		return s + 32;
	}
	return s;
}

int menu_ind = 0;

void Start_Menu(){
	inOptions = 0;
	inMainMenu = 1;
	IO_Pressed = 0;
	ST7735_FillScreen(Brown);   // set screen to 12CF
	MoveToRam(92, 42, Logo, Nidhogg);
  ST7735_DrawBitmap(38, 46, Nidhogg, 92, 42);
	
	ST7735_DrawFastVLine(34, 81, 5, White);
	ST7735_DrawFastVLine(35, 82, 3, White);
	ST7735_DrawFastVLine(36, 83, 1, White);
	                                  
	ST7735_DrawFastVLine(128, 83, 1,White);
	ST7735_DrawFastVLine(129, 82, 3,White);
	ST7735_DrawFastVLine(130, 81, 5,White);
	
	int index = 0;
	while((GPIO_PORTB_DATA_R & 0x40) == 0){
		if (menu_ind == 0){
			index = 0;
			while(options[index] != 0){
				ST7735_DrawChar(62 + 6*index, 60, options[index], Gray, Brown, 1);
				index ++;
			}
			index = 0;
			while(start[index] != 0){
				ST7735_DrawChar(54 + 12*index, 76, toUpperCase(start[index]), White, Brown, 2);
				index ++;
			}
			index = 0;
			while(credits[index] != 0){
				ST7735_DrawChar(62 + 6*index, 98, credits[index], Gray, Brown, 1);
				index ++;
			}
			IO_Wait();	
			wait_ms(8);
			if (IO_Pressed){
				break;
			}
			else if ( Joystick_Position == Left){
				menu_ind = 1;
			}
			else if (Joystick_Position == Right){
				menu_ind = 2;
			}

		}
		else if (menu_ind == 1){
			index = 0;
			while(credits[index] != 0){
				ST7735_DrawChar(62 + 6*index, 60, credits[index], Gray, Brown, 1);
				index ++;
			}
			index = 0;
			while(options[index] != 0){
				ST7735_DrawChar(42 + 12*index, 76, toUpperCase(options[index]), White, Brown, 2);
				index ++;
			}
			index = 0;
			while(start[index] != 0){
				ST7735_DrawChar(67 + 6*index, 98, start[index], Gray, Brown, 1);
				index ++;
			}	
			IO_Wait();
			wait_ms(9);
			if (IO_Pressed){
				break;
			}
			else if ( Joystick_Position == Left){
				menu_ind = 2;
			}
			else if (Joystick_Position == Right){
				menu_ind = 0;
			}
		}
		else{
			index = 0;
			while(start[index] != 0){
				ST7735_DrawChar(67 + 6*index, 60, start[index], Gray, Brown, 1);
				index ++;
			}
			index = 0;
			while(credits[index] != 0){
				ST7735_DrawChar(42 + 12*index, 76, toUpperCase(credits[index]), White, Brown, 2);
				index ++;
			}
			index = 0;
			while(options[index] != 0){
				ST7735_DrawChar(62 + 6*index, 98, options[index], Gray, Brown, 1);
				index ++;
			}	
			IO_Wait();
			wait_ms(9);
			IO_Wait();
			wait_ms(9);
			if (IO_Pressed){
				break;
			}
			else if (Joystick_Position == Left){
				menu_ind = 0;
			}
			else if (Joystick_Position == Right){
				menu_ind = 1;
			}
		}
	}
	
	if(menu_ind == 0){
		inMainMenu = 0;
		inOptions = 0;
		LobbyCreation = 1;
	}
	else if (menu_ind == 1){
		Options();
	}
	else{
		IO_Pressed = 0;
		wait_ms(100);
		inMainMenu = 0;
		inOptions = 0;
		ST7735_FillScreen(0);
		MoveToRam(20, 16, Back, BackArrow);
		ST7735_DrawBitmap(0, 16, BackArrow, 20, 16);
		ST7735_SetCursor(1,3);
		ST7735_OutString("Kunal Jain");
		ST7735_SetCursor(1,4);
		ST7735_OutString("Amrith Lotlikar");
		while(1){
		ST7735_DrawFastHLine(7, 25, 5, White);
		ST7735_DrawFastHLine(8, 24, 3, White);
		ST7735_DrawFastHLine(9, 23, 1, White);
		IO_Wait();
		IO_Wait();
		wait_ms(19);
			if (IO_Pressed){
					ST7735_DrawFastHLine(7, 25, 5, Green);
					ST7735_DrawFastHLine(8, 24, 3, Green);
					ST7735_DrawFastHLine(9, 23, 1, Green);
					wait_ms(900);
					ST7735_DrawFastHLine(7, 25, 5, White);
					ST7735_DrawFastHLine(8, 24, 3, White);
					ST7735_DrawFastHLine(9, 23, 1, White);
					return;
			}
		}
	}
	inMainMenu = 0;
}

int HeReady = 0;
int IReady = 0;

void PortF_Init(void){
	volatile int delay = 0;
	SYSCTL_RCGCGPIO_R |= 0x20; //ENABLE PORT F
	delay ++;
	delay ++;
	GPIO_PORTF_AMSEL_R &= ~0x0E;
	GPIO_PORTF_PCTL_R &= ~0x0E;
	GPIO_PORTF_DIR_R |= 0x0E;
	GPIO_PORTF_DEN_R |= 0x0E;
	GPIO_PORTF_DATA_R = 0x08;
}

uint8_t control;
uint8_t measured;
uint8_t clearfreq;
uint8_t displayed;
int32_t position100;
uint8_t position[2];
uint8_t elevation[2];
uint8_t reach[2];
uint8_t jump[3]; 
uint8_t alternate[2];
uint8_t modified;
uint8_t transmitted;
uint8_t received;
uint8_t swordposition[4];
int velocity;
int8_t redvelocity[2];
int8_t jumparr [17] = {10, 9, 7, 7, 5, 3 , 2, 1, 0, -1, -2, -3, -5, -7, -7, -9, -10};
uint8_t lungearr [7] = {19, 0, 0, 0, 0, 0, 0 };
uint8_t jumpindex[2];
uint8_t lungeindex[2];
uint8_t winner;
uint8_t dead[2];
uint8_t blocked[2]; //This is player 0. blocked[0] is 1 if I get blocked
uint8_t deadindex[2];
uint8_t gameswon[2];
uint8_t nextgame;
uint8_t over;

void Measure()
{
	if((swordposition[2] < 3) && (dead[0] == 0))
	{
			blocked[0] = 0;
			velocity = Convert(ADC_In());
			velocity -= 900;
			velocity /= 2;
		
			if(136 - position[1] <= position[0])
			{
				if(velocity < 0)
						{
							velocity = 0;
						}
							
			
			}
		
			
			if(velocity<100 && velocity>-100) velocity = 0;
			for(int i = 0; i<gameswon[0]; i++)
				{
					velocity *= .86;
				}
			
			
			
			redvelocity[0] = velocity/100;
			
		jump[2] = 1;
			
		
			if(jump[0] == 1)
			{
				jump[2] = GPIO_PORTB_DATA_R & 0x10;
				jump[2] /= 16;
			}
			
			
			if(jump[0] == 0)
			{jump[0] = GPIO_PORTB_DATA_R & 0x10;
			 jump[0] /= 16;
			}
			
			if(swordposition[2] == 2)
			{swordposition[0] = 0;}
			
			uint32_t swordvar = Convert(ADC_In2());
			if(swordvar>1500) swordposition[0] = 0;
			if(swordvar < 200) swordposition[0] =1;
			
			
			
			if(jump[0] == 0)
					{
						if((GPIO_PORTB_DATA_R & 0x40) != 0)
								{
									swordposition[0] = swordposition[2] + 3;
									if(swordposition[0] > 4) swordposition[0] =4;
								}
					
						if((GPIO_PORTB_DATA_R & 0x20) != 0)
								{
									swordposition[0] = 2;
								}
						
					}
			
		}
	measured = 1;
}

void Modify(void)
{
		if(dead[0] == 0)
		{
					if(swordposition[0] < 2)
					
					{				
									blocked[0] = 0;
									if(velocity)
											{alternate[0] += 1;
											alternate[0] %= 4;}
									if(velocity > 200 || velocity < -200)
											{alternate[0] += 2;
											alternate[0] %= 4;}
									
									
									position100 += velocity;
											
									if(position100 < 0) position100 = 0;
									if(position100>14000) position100 = 14000;
											
									position[0] = position100 / 100;

									if(jump[0] == 1)
										{
											
											if( (jumpindex[0] < 8) & (jump[2] == 0) )
											{
												jumpindex[0] = 17 - jumpindex[0];
											}
										

											if(jumpindex[0] == 17){jumpindex[0] = 0; jump[0] = 0;}
											else
												{
													elevation[0] += jumparr[jumpindex[0]];
													jumpindex[0]++;
												}
										}
					
					}
					
					if(swordposition[0]>2)
						{
							if(lungeindex[0] == 6){lungeindex[0] = 0; swordposition[0] = swordposition[2] - 3; reach[0] = 0;}
							else
								{
									reach[0] += lungearr[lungeindex[0]];
									lungeindex[0]++;
								}
						
						}
			}
		modified = 1;
	
}
uint8_t drawpack[2];
void Transmit()
{
	DisableInterrupts();
	UART_OutChar(0xFF);
	UART_OutChar(position[0]);
	UART_OutChar(redvelocity[0]);
	UART_OutChar(elevation[0]);
	drawpack[0] = 8 * jumpindex[0] + 2 * alternate[0] + jump[0];
	UART_OutChar(drawpack[0]);
	drawpack[1] = 32 * swordposition[0] + reach[0]; 
	UART_OutChar(drawpack[1]);               //available transmission space
	UART_OutChar(0x00);								//available transmission space
	UART_OutChar(0xFE);               //end
	transmitted = 1;
	control = 0;
	EnableInterrupts();
	
}

void DrawMe()
{

if(swordposition[0] < 2){
					if (alternate[0] > 1)
					{
					ST7735_DrawBitmap(position[0], 110 - elevation[0], RestYellow, 24, 27);
					
					}
					else
					{
					ST7735_DrawBitmap(position[0], 110 - elevation[0], RestYellow2, 24, 27);
					
					}
					
					if(jump[0] == 1){}
					else if(swordposition[0] == 0){
					ST7735_DrawBitmap(position[0] + 16, 93 - elevation[0], Sword, 3, 9);
					ST7735_DrawPixel(position[0] + 17, 95 - elevation[0], 0xC6F8);
					}
					else{
					ST7735_DrawBitmap(position[0] + 16, 103 - elevation[0], SwordDown, 3, 11);
					ST7735_DrawPixel(position[0] + 17, 95 - elevation[0], 0xC6F8);
					}
				}
else if (swordposition[0] == 2)
		{
				ST7735_DrawBitmap(position[0], 110, PullUp, 24, 27);
				ST7735_DrawBitmap(position[0] + 10, 85, Sword, 3, 9);
				ST7735_DrawPixel(position[0] + 11, 84, 0xC6F8);
				ST7735_FillRect(position[0] + 11, 73, 1, 15, 0xC6F8);
		 }
else if (swordposition[0] == 3)
		{
				ST7735_DrawBitmap(position[0], 110, Lunge, 24, 27);
		 }	

else
		{
				ST7735_DrawBitmap(position[0], 110, LungeLow, 24, 27);
		 }			 

	
	if(jump[0])
	{	
	if(jumparr[jumpindex[0] - 1] > 0) 
	ST7735_FillRect(position[0] - redvelocity[0], 110-elevation[0], 30, jumparr[jumpindex[0] - 1], 0x0000);
	if(jumparr[jumpindex[0] - 1] < -1) 
	ST7735_FillRect(position[0] - redvelocity[0], 110-elevation[0]- 27 + jumparr[jumpindex[0] - 1], 30, -1 * jumparr[jumpindex[0] - 1] + 1, 0x0000);
	}
}

void DrawHim()
{
if(swordposition[1] < 2)
	{
	if (alternate[1] > 1)
	{
	ST7735_DrawBitmap(136 - position[1], 110 - elevation[1], RestBlue, 24, 27);
	}
	else
	{
	ST7735_DrawBitmap(136 - position[1], 110 - elevation[1], RestBlue2, 24, 27);	   
	}
					
					if(jump[1] == 1)
					{
						
					}
					else if(swordposition[1] == 0){
					ST7735_DrawBitmap(136 - position[1] + 5, 93 - elevation[1], HisSword, 3, 9);
					ST7735_DrawPixel(136 - position[1] + 6, 95 - elevation[1], 0xC6F8);
					}
					else{
					ST7735_DrawBitmap(136 - position[1] + 5, 103 - elevation[1], HisSwordDown, 3, 11);
					ST7735_DrawPixel(136 - position[1] + 6, 97 - elevation[1], 0xC6F8);
					}
	
	
}
	
	else if (swordposition[1] == 2)
		{
			ST7735_DrawBitmap(136 - position[1], 110, HePullUp, 24, 27);
			ST7735_DrawBitmap(147 - position[1], 85, Sword, 3, 9);
			ST7735_DrawPixel(148 - position[1], 84, 0xC6F8);
			ST7735_FillRect(148 - position[1], 73, 1, 15, 0xC6F8);
		
		}	
		
	else if(swordposition[1] == 3)
		{
			ST7735_DrawBitmap(136 - position[1], 110, HeLunge, 24, 27);
		
		}
	else 
		{
			ST7735_DrawBitmap(136 - position[1], 110, HeLungeLow, 24, 27);
		
		}
		
		
		if(jump[1])
	{	
	if(jumparr[jumpindex[1] - 1] > 0) 
	ST7735_FillRect(136 - position[1] + redvelocity[1], 110-elevation[1], 30, 2 + elevation[1], 0x0000);
	if(jumparr[jumpindex[1] - 1] < -1) 
	ST7735_FillRect(136 - position[1] + redvelocity[1], 40, 30, 50 - elevation[1], 0x0000);
	}

}

void CleanUp()
{
if(swordposition[2] == 2 || swordposition[3] == 2)
					{
					ST7735_FillRect(position[0] -3, 73, 30, 15, 0x000);
					ST7735_FillRect(136 - position[1] -3, 73, 30, 15, 0x000);
					}	
				
			
	if(swordposition[2] == 3) ST7735_FillRect(position[0] + 24, 92, 25, 1, 0x0000);
	if(swordposition[2] == 4) ST7735_FillRect(position[0] + 24, 99, 25, 1, 0x0000);
	if(swordposition[3] == 3) ST7735_FillRect(136 - position[1] -25, 92, 25, 1, 0x0000);
	if(swordposition[3] == 4) ST7735_FillRect(136 - position[1] - 25, 99, 25, 1, 0x0000);
			if(deadindex[0] == 0)
			{
				if(dead[0] == 1) deadindex[0]++;
			}
		else 
			{
				if(deadindex[0] < 20) deadindex[0]++;
			}
	if(deadindex[0] == 1)
			{
					ST7735_FillRect(position[0] - 6, 75 - elevation[0], 36, 35 + elevation[0],0x0000);
			}
	if(deadindex[1] == 0)
			{
				if(dead[1] == 1) deadindex[1]++;
			}
		else 
			{
				if(deadindex[1] < 20) deadindex[1]++;
			}
	if(deadindex[1] == 1)
			{
					ST7735_FillRect(136 - position[1] - 6, 75 - elevation[1], 36, 35 + elevation[1],0x0000);
			}



}
void ShowStatus()
{
	ST7735_SetCursor(0,0);
	ST7735_OutString("You: ");
	LCD_OutDec(gameswon[0]);
	ST7735_SetCursor(12, 0);
	ST7735_OutString("Them: ");
	LCD_OutDec(gameswon[1]);
	ST7735_SetCursor(3,2);
	
	if(winner == 1)
	{ST7735_OutString("Y'won this round");}
	else if(winner == 2)
	{ST7735_OutString("Y'lost this round");}
	else if(dead[1] == 1)
		{
			ST7735_OutString("You murderer!");
		}
	else if(dead[0] == 1)
		{
			ST7735_OutString("You ded fam! ");
		}
	else if (blocked[0])
	{
			ST7735_OutString("Rejected!    ");
	}
	else if (blocked[1])
	{
			ST7735_OutString("Nice block!! ");
	}

}
void Display()
{
			
			if(transmitted && measured && modified)
			{
			
			CleanUp();
							
					
			if(dead[0] == 0)			
			{DrawMe();}
			if(dead[1] == 0)
			{DrawHim();}
			
			if(dead[0] == 0 && blocked[0] == 0)
			{
			if(swordposition[0] == 3) ST7735_FillRect(position[0] + 24, 92, reach[0], 1, 0xC6F8);
			if(swordposition[0] == 4) ST7735_FillRect(position[0] + 24, 99, reach[0], 1, 0xC6F8);
			}
			if(dead[1] == 0 && blocked[1] == 0)
			{
			if(swordposition[1] == 3) ST7735_FillRect(136 - position[1] -reach[1], 92, reach[1], 1, 0xC6F8);
			if(swordposition[1] == 4) ST7735_FillRect(136 - position[1] - reach[1], 99, reach[1], 1, 0xC6F8);
			}
			
			
			

			swordposition[2] = swordposition[0];
			swordposition[3] = swordposition[1];
			
			ShowStatus();
			
			
			if(winner != 0) nextgame = 1;
			
			
			}
			measured  = 0;
			modified = 0;
			transmitted = 0;
			displayed = 1;
		}


void Initialize_Game()
	{
		ST7735_FillRect(0, 0, 160, 112, 0x0000);
		ST7735_FillRect(0, 112, 160, 16,0x0911);
		ST7735_SetCursor(0,0);
		ST7735_SetTextColor(me_color);
		ST7735_OutString("You: ");
		LCD_OutDec(gameswon[0]);
		ST7735_SetCursor(12, 0);
		ST7735_SetTextColor(enemy_color);
		ST7735_OutString("Them: ");
		ST7735_SetTextColor(White);
		LCD_OutDec(gameswon[1]);
		position100 = 0;
		position[0] = 0;
		position[1] = 0;
		elevation[0] = 0;
		elevation[1] = 0;
		jump[0] = 0;
		jump [1] = 0;
		redvelocity[1] = 0;
		redvelocity[0] = 0;
		jumpindex[0] = 0;
		jumpindex[1] = 0;
		alternate[0] = 0;
		swordposition[0] = 0;
		swordposition[3] = 0;
		swordposition[2] = 0;
		lungeindex[0] = 0;
		alternate[1] = 0;
		
		nextgame = 0;
		measured = 0;
		modified = 0;
		transmitted = 0;
		displayed = 1;  
		control = 1;
		
		dead[0] = 0;
		dead[1] = 0;
		blocked[0] = 0;
		blocked[1] = 0;
		winner = 0;
		deadindex[0] = 0;
		deadindex[1] = 0;
	}
void GameOver()
{
	ST7735_FillScreen(0x0000);
	if(gameswon[0] == 5)
	{
		ST7735_SetCursor(5,5);
		ST7735_OutString("Congratulations!");
		ST7735_SetCursor(5,6);
		ST7735_OutString("You Won!");
	}
	else
	{
		ST7735_SetCursor(5,5);
		ST7735_OutString("I'm sorry.");
		ST7735_SetCursor(5,6);
		ST7735_OutString("You lost.");	
	}


}
	void StartNewRound()
		{
			DisableInterrupts();
			if(winner == 1){gameswon[0]++;}
			if(winner == 2){gameswon[1]++;}
			if(gameswon[0] == 5 || gameswon[1] == 5){over = 1;}
			if(!over)
			{	
			volatile int delay = 0;
			for(int i = 1; i< 10000000; i++)
			{delay++;}
			Initialize_Game();
			for(int i = 1; i< 10000000; i++)
			{delay++;}
			ST7735_SetCursor(3,2);
			ST7735_OutString("Go!          ");
			EnableInterrupts();
		}
		}
	void PlayGame()
	{
		gameswon[0] = 0;
		gameswon[1] = 0;
		over = 0;
		Initialize_Game();
		
		Recolor_R(24, 29, WalkLegsClose, RestYellow, Orange, me_color);
		Recolor_R(24, 29, WalkLegsOpen, RestYellow2, Orange, me_color);
		Recolor_R(24, 29, LungeUp, PullUp, Orange, me_color);
		Recolor_R(24, 29, HighLunge, Lunge, Orange, me_color);
		Recolor_R(24, 29, LowLunge, LungeLow, Orange, me_color);
		MoveToRam(3, 9, UpSword, Sword);
		MoveToRam(3, 9, UpSword, HisSword);
		MoveToRam_Down_Sword();
		MoveToRam_Left(enemy_color);
	
		
		EnableInterrupts();
		while(!over)
		{
			if(!displayed){Display();}
			if(nextgame) StartNewRound();
			if(!over)
			{
			if(control && !measured){Measure();}
			if(control && measured && !modified){Modify();}
			if(control && measured && modified && !transmitted){Transmit();}
			}
			if(over) GameOver();
		}
	
	
	}
	
	uint8_t filler;
	uint8_t unpackaging[3];
void UART1_Handler(void){
	GPIO_PORTF_DATA_R ^= 0x04;					// PF1 - PF3 are heartbeats
	GPIO_PORTF_DATA_R ^= 0x04;					// PF1 - PF3 are heartbeats
			  while((uint8_t)UART_InChar() != 0xFF){}
				position[1] = UART_InChar();
				redvelocity[1] = UART_InChar();
				elevation[1] = UART_InChar();
				unpackaging[0] = UART_InChar();
					
				jumpindex[1] = unpackaging[0]/8;
				alternate[1] = (unpackaging[0] % 8)/2;
				jump[1] = unpackaging[0] % 2;
					
				unpackaging[1] = UART_InChar();
				swordposition[1] = unpackaging[1]/32;
				reach[1] = unpackaging[1] % 32;
				unpackaging[2] = UART_InChar();
				
				blocked[0] = unpackaging[2] % 2;
				unpackaging[2] /= 2;
				blocked[1] = unpackaging[2] % 2;
				unpackaging[2] /= 2;
				dead[0] = unpackaging[2] % 2;
				unpackaging[2] /= 2;
				dead[1] = unpackaging[2] % 2;
				winner = (unpackaging[2] / 2)%4;
				
				
				
				filler = UART_InChar();
				UART1_ICR_R = 0x10;
				GPIO_PORTF_DATA_R ^= 0x04;					// PF1 - PF3 are heartbeats
				control = 1;
		}


int main(void){
  PLL_Init(Bus80MHz);       						// Bus clock is 80 MHz 
	Timer0_Init();
	UART_Init();
	PortF_Init();
	Sound_Init();
	ADC_Init();
	Buttons_Init();
  Output_Init();												// Calls ST7735_InitR(INITR_REDTAB) and sets the Screen to black;
	ST7735_SetRotation(1);
	while(!LobbyCreation){
		Start_Menu();
	}
	PlayGame();
}
