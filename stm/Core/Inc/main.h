
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32l5xx_hal.h"
#include "stm32l5xx_nucleo.h"

// File Follower Friend States

typedef enum { STATE_MENU, STATE_DIRS } FFFState;
typedef enum { MENU_ACCESS_DIRS, MENU_UPD_BRIGHT } FFFMenu;

typedef struct {
  uint16_t vert;
  uint16_t horz;
} Joystick;

typedef struct {
  int top;
  int bottom;
  int joystick;
} Buttons;

typedef struct {
  uint16_t bg;
  uint16_t text;
  uint16_t dir;
  uint16_t cursor;
} Colors;

typedef struct {
  FFFState state;
  FFFMenu menuState;
  Joystick joystick;
  Buttons buttons;
  Colors colors;
  int brightness;
  int currentY;
  int currentX;
  int render;
} FFFContext;

extern FFFContext gCtx;

/**
 * Gets the directory from a string containing all
 * content in the current directory.
 *
 * @param dir The directory to get
 * @param dirs The content of the current directory
 *
 * @return NULL if the specified directory is not
 * a directory, a string representing the relative
 * path of the directory otherwise.
 */
const char *getDirectory(int dir, const char *dirs);

/**
 * Gets the number of characters that make
 * up a directory. Terminators are newline
 * and the null-terminator.
 *
 * @param dir The directory to get the length for
 *
 * @return The length of the directory
 */
int getDirectoryLength(char *dir);

#ifdef __cplusplus
}
#endif

#endif
