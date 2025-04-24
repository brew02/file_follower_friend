
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32l5xx_hal.h"
#include "stm32l5xx_nucleo.h"
#include <stdio.h>

typedef enum { STATE_MENU, STATE_DIRS } FFFState;
typedef enum { MENU_ACCESS_DIRS, MENU_UPD_BRIGHT } FFFMenu;

extern FFFState state;
extern FFFMenu menu;

#ifdef __cplusplus
}
#endif

#endif
