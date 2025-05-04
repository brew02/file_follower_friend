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