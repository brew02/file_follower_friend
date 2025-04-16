#include "timer.h"
#include "stm32l552xx.h"

void delayMS(uint32_t val) {
  SysTick->CTRL &= ~(1 << 0); // Disable SysTick
  SysTick->LOAD = 16000 - 1;  // Period of 1 MS (16MHz / 16000)
  SysTick->VAL = 0;           // Reset counter
  SysTick->CTRL |= (1 << 2) | // Use processor clock
                   (1 << 0);  // Enable SysTick

  for (unsigned int i = 0; i < val; ++i) {
    // Wait until COUNTFLAG is 1 (automatically reset to 0 on reads)
    while (((SysTick->CTRL >> 16) & 1) == 0)
      ;
  }
}

void resetTIM1Count() {
  TIM1->CR1 &= ~(1 << 0); // Disable TIM1
  TIM1->EGR |= (1 << 0);  // Update generation
  TIM1->SR &= ~(1 << 0);  // Clear update interrupt flag
  TIM1->CR1 |= (1 << 0);  // Enable TIM1
}

void initTimers() {
  TIM1->PSC = 16000 - 1;  // Period of 1 MS (16Mhz / 16000)
  TIM1->ARR = 10;         // Count for 10 MS
  TIM1->CNT = 0;          // Reset counter
  TIM1->DIER |= (1 << 0); // Enable update interrupt
  TIM1->SR &= ~(1 << 0);  // Clear update interrupt flag
  TIM1->CR1 |=
      (1 << 3) | // Enable one-pulse mode
      (1 << 2);  // Only counter overflow/undeflow generates an update interrupt

  NVIC_SetPriority(TIM1_UP_IRQn, 0); // Set interrupt priority to 0 (max)
  NVIC_EnableIRQ(TIM1_UP_IRQn);      // Enable TIM1 IRQ

  TIM3->PSC = 0;              // Period of 62.5 NS (16MHz / 1)
  TIM3->ARR = 100 - 1;        // Count 100 times
  TIM3->CCMR1 = (0b110 << 4); // Active for TIM3_CNT < TIM3_CCR1
  TIM3->CCR1 = 100;           // Capture/compare value for channel 1
  TIM3->CCER = (1 << 0);      // Enable capture/compare for channel 1
  TIM3->CNT = 0;              // Reset counter

  TIM3->CR1 = (1 << 0); // Enable TIM3
}

void TIM1_UP_IRQHandler() {

  // TODO: Update LCD display

  TIM1->SR &= ~(1 << 0); // Clear update interrupt flag
}
