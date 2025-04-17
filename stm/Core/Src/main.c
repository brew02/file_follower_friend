#include "lcd.h"
#include "spi.h"
#include "stm32l552xx.h"
#include "timer.h"

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
  GPIOE 15 (SPI1_MOSI)  <-> J2.15 (LCD SDA/SPI MOSI)
  GPIOE 13 (SPI1_SCK)   <-> J1.7 (LCD SPI CLK)
  GPIOE 12              <-> J2.13 (LCD SPI CS)
  GPIOE 11              <-> J4.31 (LCD RS)
  GPIOE 10              <-> J2.17 (LCD RST)
  GPIOE 3  (TIM3 PWM)   <-> J4.39 (LCD BACKLIGHT)
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

int main() {
  enableClocks();
  initGPIOs();
  initLPUART1();
  initTimers();
  initSPI1();
  initLCD();

  while (1) {
    useLCD();
  }

  return 0;
}
