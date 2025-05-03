/**
 * This file contains functions to initialize
 * the timers, handle timer interrupts, update
 * timers, and use timers to delay.
 *
 * @author Brodie Abrew & Lucas Berry
 */

#include "timer.h"
#include "bitmacro.h"
#include "stm32l552xx.h"

void delayMS(unsigned long val) {
  (void)BITCHECK((*(volatile unsigned long *)&SysTick->CTRL),
                 16);         // Clears COUNTFLAG
  BITCLEAR(SysTick->CTRL, 0); // Disable SysTick
  SysTick->LOAD = 16000 - 1;  // Period of 1 MS (16MHz / 16000)
  SysTick->VAL = 0;           // Reset counter
  BITSET(SysTick->CTRL, 2);   // Use processor clock
  BITSET(SysTick->CTRL, 0);   // Enable SysTick

  for (unsigned int i = 0; i < val; ++i) {
    // Wait until COUNTFLAG is 1 (automatically reset to 0 on reads)
    while (BITCHECK(SysTick->CTRL, 16) == 0)
      ;
  }
}

void initTimers(int pwm) {
  TIM3->CR1 = 0;
  TIM3->PSC = 1;          // Period of 125 NS (16MHz / 2)
  TIM3->ARR = 100 - 1;    // Count 100 times
  BITSET(TIM3->CCMR1, 6); // Active for TIM3_CNT < TIM3_CCR1
  BITSET(TIM3->CCMR1, 5);
  BITCLEAR(TIM3->CCMR1, 4);
  TIM3->CCR1 = pwm;      // Capture/compare value for channel 1
  BITSET(TIM3->CCER, 0); // Enable capture/compare for channel 1
  TIM3->CNT = 0;         // Reset counter
  BITSET(TIM3->CR1, 0);  // Enable TIM3
}

void updateTIM3PWM(int pwm) {
  BITCLEAR(TIM3->CR1, 0); // Disable TIM3
  BITSET(TIM3->EGR, 0);   // Update generation
  BITCLEAR(TIM3->SR, 0);  // Clear update interrupt flag
  TIM3->CCR1 = pwm;       // Capture/compare value for channel 1
  BITSET(TIM3->CR1, 0);   // Enable TIM3
}
