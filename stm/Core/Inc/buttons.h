/**
 * This file exposes the functions for
 * controlling the various buttons.
 *
 * @author Brodie Abrew & Lucas Berry
 */

#ifndef __BUTTONS_H
#define __BUTTONS_H

#include <stdint.h>

#define MAX_BUF_SIZE 1024 // Max string length

extern int topButton;
extern int bottomButton;

extern char txBuffer[MAX_BUF_SIZE];
extern int txIndex;
extern int txInProgress;

/**
 * Initializes external interrupts for various buttons.
 */
void initButtons();

#endif
