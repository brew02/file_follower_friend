#include "timer.h"
#include "bitmacro.h"
#include "stm32l552xx.h"

void delayMS(uint32_t val) {
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

void resetTIM1Count() {
  BITCLEAR(TIM1->CR1, 0); // Disable TIM1
  BITSET(TIM1->EGR, 0);   // Update generation
  BITCLEAR(TIM1->SR, 0);  // Clear update interrupt flag
  BITSET(TIM1->CR1, 0);   // Enable TIM1
}

void initTimers() {
  TIM1->PSC = 16000 - 1; // Period of 1 MS (16Mhz / 16000)
  TIM1->ARR = 10;        // Count for 10 MS
  TIM1->CNT = 0;         // Reset counter
  BITSET(TIM1->DIER, 0); // Enable update interrupt
  BITCLEAR(TIM1->SR, 0); // Clear update interrupt flag
  BITSET(TIM1->CR1, 3);  // Enable one-pulse mode
  BITSET(TIM1->CR1,
         2); // Only counter overflow/underflow generates an update interrupt

  NVIC_SetPriority(TIM1_UP_IRQn, 0); // Set interrupt priority to 0 (max)
  NVIC_EnableIRQ(TIM1_UP_IRQn);      // Enable TIM1 IRQ

  TIM3->PSC = 1;          // Period of 125 NS (16MHz / 2)
  TIM3->ARR = 100 - 1;    // Count 100 times
  BITSET(TIM3->CCMR1, 6); // Active for TIM3_CNT < TIM3_CCR1
  BITSET(TIM3->CCMR1, 5);
  BITCLEAR(TIM3->CCMR1, 4);
  TIM3->CCR1 = 50;       // Capture/compare value for channel 1
  BITSET(TIM3->CCER, 0); // Enable capture/compare for channel 1
  TIM3->CNT = 0;         // Reset counter
  BITSET(TIM3->CR1, 0);  // Enable TIM3
}

void TIM1_UP_IRQHandler() {

  // TODO: Update LCD display

  BITCLEAR(TIM1->SR, 0); // Clear update interrupt flag
}
