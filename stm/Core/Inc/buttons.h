/**
 * This file exposes the functions for
 * controlling the various buttons.
 *
 * @author Brodie Abrew & Lucas Berry
 */

#ifndef __BUTTONS_H
#define __BUTTONS_H

#include <stdint.h>

extern int topButton;
extern int bottomButton;

/**
 * Initializes external interrupts for various buttons.
 */
void initButtons();

#endif
