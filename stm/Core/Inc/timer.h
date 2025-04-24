/**
 * This file exposes the functions for
 * controlling timers.
 *
 * @author Brodie Abrew & Lucas Berry
 */

#ifndef __TIME_H
#define __TIME_H

#include <stdint.h>

/**
 * Delays the CPU for the number of milliseconds
 * specified.
 *
 * @param val The number of milliseconds to delay
 */
void delayMS(uint32_t val);

/**
 * Resets the TIM1 counter value to 0.
 */
void resetTIM1Count();

/**
 * Initializes TIM1 and TIM3.
 */
void initTimers();

#endif