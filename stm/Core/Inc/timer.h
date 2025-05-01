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
void delayMS(unsigned long val);

/**
 * Sets SysTick to "count down" for the specified
 * number of milliseconds.
 *
 * @param val The number of milliseconds to
 * "count down" from
 */
void setSysTickCountdown(unsigned long val);

/**
 * Resets the TIM1 counter value to 0.
 */
void resetTIM1Count();

/**
 * Initializes TIM3 with the PWM value specified.
 *
 * @param pwm The duty cycle
 */
void initTimers(int pwm);

/**
 * Updates TIM3 with the PWM value specified.
 *
 * @param pwm The duty cycle
 */
void updateTIM3PWM(int pwm);

#endif