/**
 * This file exposes the functions for
 * controlling the LCD screen on the BOOSTXL-EDUMKII.
 *
 * @author Brodie Abrew & Lucas Berry
 */

#ifndef __LCD_H
#define __LCD_H

#include <stdint.h>

#include "main.h"

// Crystalfontz CFAF128128B-0145T
enum CFAF128128B_0145T {
  CFAF_HEIGHT = 129,
  CFAF_WIDTH = 127,
};

enum LCD_CHARS {
  // Five horizontal pixels per character
  PIXELX = 5,
  // Eight vertical pixels per character
  PIXELY = 8,
  // One extra space so characters don't collide
  PIXEL_SPACEX = PIXELX + 1,
  PIXEL_SPACEY = PIXELY + 1,
  // Maximum character limits
  LIMITX = ((CFAF_WIDTH + 1) / PIXEL_SPACEX) - 1,
  LIMITY = ((CFAF_HEIGHT + 1) / PIXEL_SPACEY) - 1
};

/**
 * Initializes the LCD screen on the BOOSTXL-EDUMKII.
 *
 * @param bgColor - The initial background color
 */
void initLCD(uint16_t bgColor);

/**
 * Renders a filled rectangle to the LCD screen on the BOOSTXL-EDUMKII.
 *
 * @param sX The horizontal starting coordinate of the rectangle
 * @param sY The vertical starting coordinate of the rectangle
 * @param eX The horizontal ending coordinate of the rectangle
 * @param eY The vertical ending coordinate of the rectangle
 * @param color The color of the filled rectangle
 */
void renderFilledRectangle(int sX, int sY, int eX, int eY, uint16_t color);

/**
 * Renders an image to the LCD screen on the
 * BOOSTXL-EDUMKII.
 *
 * @param buffer The image represented as 16-bit pixels
 * @param size The total size of the image in bytes
 */
void renderImage(const char *buffer, int size);

/**
 * Renders a character to the LCD screen on the BOOSTXL-EDUMKII.
 *
 * @param x The horizontal starting coordinate of the character
 * @param y The vertical starting coordinate of the character
 * @param c The character to render
 * @param charColor The color for the character
 * @param bgColor The color for the background
 */
void renderChar(int x, int y, char c, uint16_t charColor, uint16_t bgColor);

/**
 * Renders a string of characters to the LCD screen on the
 * BOOSTXL-EDUMKII safely.
 *
 * @param x The horizontal starting coordinate of the character
 * @param y The vertical starting coordinate of the character
 * @param size The maximum number of characters to print
 * @param text The string of characters to render
 * @param textColor The color for the text
 * @param bgColor The color for the background
 *
 * @return The number of characters rendered
 */
unsigned long renderStringSafe(int x, int y, int size, const char *text,
                               uint16_t textColor, uint16_t bgColor);

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
unsigned long renderString(int x, int y, const char *text, uint16_t textColor,
                           uint16_t bgColor);

/**
 * Renders the directories to the LCD screen on the
 * BOOSTXL-EDUMKII.
 *
 * @param current The current directory we are on
 * @param dirs The directories as a null-terminated, newline separated string
 * @param cursorColor The color for the cursor
 * @param dirColor The color for printing directories
 * @param textColor The color for the text
 * @param bgColor The color for the background
 *
 * @return The number of characters rendered
 */
unsigned long renderDirectories(int current, const char *dirs,
                                uint16_t cursorColor, uint16_t dirColor,
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

/**
 * Renders the menu if in the menu state.
 *
 * @param ctx The File Follower Friend context
 */
void renderMenu(FFFContext *ctx);

#endif