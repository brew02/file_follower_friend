#include "spi.h"
#include "stm32l552xx.h"

void enableClocks() {
  RCC->APB1ENR1 |= (1 << 28) | // Power interface clock enable
                   (1 << 1);   // TIM3 clock enable

  RCC->AHB2ENR |= (1 << 6) | // GPIOG clock enable
                  (1 << 4) | // GPIOE clock enable
                  (1 << 2);  // GPIOC clock enable

  RCC->APB1ENR2 |= (1 << 0); // LPUART1 clock enable

  RCC->APB2ENR |= (1 << 12) | // SPI1 clock enable
                  (1 << 11);  // TIM1 clock enable

  RCC->CCIPR1 |= (0b11 << 10); // HSI16 clock for LPUART1

  RCC->CFGR |= (1 << 0); // HSI16 clock for SYSCLK

  RCC->CR |= (1 << 8) |    // HSI16 clock enable
             (0b11 << 5) | // MSI clock at 4MHz
             (1 << 0);     // MSI clock enable
}

/*
  GPIOE 15 - SPI1_MOSI
  GPIOE 13 - SPI1_SCK
  GPIOE 12 - SPI CS
  GPIOE 11 - LCD RS
  GPIOE 10 - LCD RST
*/

void initGPIOs() {
  PWR->CR2 |= (1 << 9); // Enable power to GPIOG

  GPIOG->MODER |= (0b10 << 16) | // Set GPIOG 8 to AF
                  (0b10 << 14);  // Set GPIOG 7 to AF

  GPIOG->AFR[1] |= (0b1000 << 0);  // Set GPIOG 8 to LPUART1_RX
  GPIOG->AFR[0] |= (0b1000 << 28); // Set GPIOG 7 to LPUART1_TX

  GPIOE->MODER |= (0b10 << 30) | // Set GPIOE 15 to AF
                  (0b10 << 26) | // Set GPIOE 13 to AF
                  (0b01 << 24) | // Set GPIOE 12 to output
                  (0b01 << 22) | // Set GPIOE 11 to output
                  (0b01 << 20) | // Set GPIOE 10 to output
                  (0b10 << 6);   // Set GPIOE 3 to AF

  GPIOE->OSPEEDR |= (0b11 << 30) | // Set GPIOE 15 to very high speed
                    (0b11 << 26);  // Set GPIOE 13 to very high speed

  GPIOE->AFR[1] |= (0b0101 << 28) | // Set GPIOE 15 to SPI1_MOSI
                   (0b0101 << 20);  // Set GPIOE 13 to SPI1_SCK

  GPIOE->AFR[0] |= (0b0010 << 12); // Set GPIOE 3 to TIM3_CH1
}

void resetTIM1Count() {
  TIM1->CR1 &= ~(1 << 0); // Disable TIM1
  TIM1->EGR |= (1 << 0);  // Update generation
  TIM1->SR &= ~(1 << 0);  // Clear update interrupt flag
  TIM1->CR1 |= (1 << 0);  // Enable TIM1
}

void LPUART1_IRQHandler() {
  // Check RXNE flag
  if (((LPUART1->ISR >> 5) & 1) == 1) {

    // TODO: Store each character in a message buffer

    // Reset the counter (receiving a continuous message)
    resetTIM1Count();
  }
}

void initLPUART1() {
  LPUART1->BRR = 35555;      // BAUD rate of 115200 (256 * 16Mhz / 115200)
  LPUART1->CR1 |= (1 << 5) | // Enable RXFIFO not empty interrupt
                  (1 << 3) | // Enable transmitter
                  (1 << 2) | // Enable receiver
                  (1 << 0);  // Enable LPUART1

  NVIC_SetPriority(LPUART1_IRQn, 0); // Set interrupt priority to 0 (max)
  NVIC_EnableIRQ(LPUART1_IRQn);      // Enable LPUART1 IRQ
}

void TIM1_UP_IRQHandler() {

  // TODO: Update LCD display

  TIM1->SR &= ~(1 << 0); // Clear update interrupt flag
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

  TIM3->PSC = 0;       // Period of 62.5 NS (16MHz / 1)
  TIM3->ARR = 100 - 1; // Count 100 times

  TIM3->CCMR1 = (0b110 << 4); // Active for TIM3_CNT < TIM3_CCR1
  TIM3->CCR1 = 100;           // Capture/compare value for channel 1

  TIM3->CCER = (1 << 0); // Enable capture/compare for channel 1

  TIM3->CNT = 0;        // Reset counter
  TIM3->CR1 = (1 << 0); // Enable TIM3
}

int main() {
  enableClocks();
  initGPIOs();
  initLPUART1();
  initTimers();
  initSPI1();

  while (1) {
  }

  return 0;
}
