// Sound.h
// Runs on TM4C123 or LM4F120
// Allows for the creation of songs based on notes and western classical music scoring
// Kunal Jain and Amrith Lotlikar
// May 1, 2018

#ifndef __SOUND_H__ // do not include more than once
#define __SOUND_H__

void Sound_Init(void);
void Sound_Play(uint32_t period);

// Period =  80000000/64/Freq= 1250000/Freq			
#define C1   597   // 2093 Hz			
#define B1   633   // 1975.5 Hz			
#define BF1  670   // 1864.7 Hz			
#define A1   710   // 1760 Hz			
#define AF1  752   // 1661.2 Hz			
#define G1   797   // 1568 Hz			
#define GF1  845   // 1480 Hz			
#define F1   895   // 1396.9 Hz			
#define E1   948   // 1318.5 Hz			
#define EF1  1004   // 1244.5 Hz			
#define D1   1064   // 1174.7 Hz			
#define DF1  1127   // 1108.7 Hz			
#define C    1194   // 1046.5 Hz			
#define B    1265   // 987.8 Hz			
#define BF   1341   // 932.3 Hz			
#define A    1420   // 880 Hz			
#define AF   1505   // 830.6 Hz			
#define G    1594   // 784 Hz			
#define GF  1689   // 740 Hz			
#define F   1790   // 698.5 Hz			
#define E   1896   // 659.3 Hz			
#define EF  2009   // 622.3 Hz			
#define D   2128   // 587.3 Hz			
#define DF  2255   // 554.4 Hz			
#define C0  2389   // 523.3 Hz			
#define B0  2531   // 493.9 Hz			
#define BF0 2681   // 466.2 Hz			
#define A0  2841   // 440 Hz			
#define AF0 3010   // 415.3 Hz			
#define G0  3189   // 392 Hz			
#define GF0 3378   // 370 Hz			
#define F0  3579   // 349.2 Hz			
#define E0  3792   // 329.6 Hz			
#define EF0 4018   // 311.1 Hz			
#define D0  4257   // 293.7 Hz			
#define DF0 4510   // 277.2 Hz			
#define C7  4778   // 261.6 Hz			
#define B7  5062   // 246.9 Hz			
#define BF7 5363   // 233.1 Hz			
#define A7  5682   // 220 Hz			
#define AF7 6020   // 207.7 Hz			
#define G7  6378   // 196 Hz			
#define GF7 6757   // 185 Hz			
#define F7  7159   // 174.6 Hz			
#define E7  7584   // 164.8 Hz			
#define EF7 8035   // 155.6 Hz			
#define D7  8513   // 146.8 Hz			
#define DF7 9019   // 138.6 Hz			
#define C6  9556   // 130.8 Hz

#define ONETWENTY8	  1250000					* 0.8
#define SIXTYFOURTH		2500000 				* 0.8
#define THIRTYSECOND	5000000					* 0.8
#define SIXTEENTH 		10000000 				* 0.8
#define TRIPLET 			13333333 				* 0.8
#define EIGHTH 				20000000 				* 0.8
#define QUARTER 			40000000 				* 0.8
#define HALF 					80000000 				* 0.8
#define WHOLE 				160000000 			* 0.8

#endif // __SOUND_H__
