#ifndef __LCD_H
#define __LCD_H

#include <stdint.h>

// Crystalfontz CFAF128128B-0145T
enum CFAF128128B_0145T {
  CFAF_HEIGHT = 128,
  CFAF_WIDTH = 129,
};

void initLCD();

void renderChar(uint8_t x, uint8_t y, char c, uint16_t textColor,
                uint16_t bgColor, uint8_t size);
unsigned long renderString(uint8_t x, uint8_t y, const char *text,
                           uint16_t textColor, uint16_t bgColor);
uint16_t color24to16(uint8_t r, uint8_t g, uint8_t b);

void test();

#endif