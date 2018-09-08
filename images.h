// images.h
// Runs on LM4F120/TM4C123
// holds key game bitmaps functions
// Kunal Jain and Amrith Lotlikar
// May 1, 2018

#ifndef __IMAGES_H__
#define __IMAGES_H__

//------------MoveToRam------------
// Input: Bit map width, height, ROM array, RAM array
// Output: none
// Puts the bitmap in reverse order into RAM
void MoveToRam(uint32_t x, uint32_t y, const unsigned short bitmap_origin[], volatile unsigned short bitmap_destination[]);

//------------Recolor------------
// Input: Bit map width, height, ROM array, RAM array, original bitmap's color, new bitmap's color
// Output: none
// Puts the bitmap in reverse order into RAM, wodth the new color
void Recolor(uint32_t x, uint32_t y, const unsigned short bitmap_origin[], volatile unsigned short bitmap_destination[], uint16_t Orig_color, uint16_t new_color);

void Recolor_R(uint32_t x, uint32_t y, const unsigned short bitmap_origin[], volatile unsigned short bitmap_destination[], uint16_t Orig_color, uint16_t new_color);

void Recolor_L(uint32_t x, uint32_t y, const unsigned short bitmap_origin[], volatile unsigned short bitmap_destination[], uint16_t Orig_color, uint16_t new_color);

void MoveToRam_Left(uint16_t color);

void MoveToRam_Down_Sword(void);
#endif // __IMAGES_H__
