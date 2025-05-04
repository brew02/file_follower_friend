
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
  int render;
} FFFContext;

extern FFFContext gCtx;

#ifdef __cplusplus
}
#endif

#endif
