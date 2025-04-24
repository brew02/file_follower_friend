/**
 * This file exposes the functions for
 * controlling the LCD screen on the BOOSTXL-EDUMKII.
 *
 * @author Brodie Abrew & Lucas Berry
 */

#ifndef __LCD_H
#define __LCD_H

#include <stdint.h>

// Crystalfontz CFAF128128B-0145T
enum CFAF128128B_0145T {
  CFAF_HEIGHT = 127,
  CFAF_WIDTH = 127,
};

/**
 * Initializes the LCD screen on the BOOSTXL-EDUMKII.
 */
void initLCD();

/**
 * Renders a character to the LCD screen on the BOOSTXL-EDUMKII.
 *
 * @param x The horizontal starting coordinate of the character
 * @param y The vertical starting coordinate of the character
 * @param c The character to render
 * @param charColor The color for the character
 * @param bgColor The color for the background
 */
void renderChar(uint8_t x, uint8_t y, char c, uint16_t charColor,
                uint16_t bgColor);

/**
 * Renders a string of characters to the LCD screen on the
 * BOOSTXL-EDUMKII.
 *
 * @param x The horizontal starting coordinate of the character
 * @param y The vertical starting coordinate of the character
 * @param text The string of characters to render
 * @param textColor The color for the text
 * @param bgColor The color for the background
 *
 * @return The number of characters rendered
 */
unsigned long renderString(uint8_t x, uint8_t y, const char *text,
                           uint16_t textColor, uint16_t bgColor);

/**
 * Converts a 24-bit color to a 16-bit color.
 * (Note: Color precision is lost)
 *
 * @param r The 8-bit red value to convert
 * @param g The 8-bit green value to convert
 * @param b The 8-bit blue value to convert
 *
 * @return The converted 16-bit color
 */
uint16_t color24to16(uint8_t r, uint8_t g, uint8_t b);

#endif