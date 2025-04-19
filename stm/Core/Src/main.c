#include "bitmacro.h"
#include "lcd.h"
#include "spi.h"
#include "stm32l552xx.h"
#include "timer.h"

#define MAX_BUF_SIZE 1024 // Max string length
int flag = 0;             // Flag for UART checking
char buf[MAX_BUF_SIZE];   // Buffer to store received string
int bufIndex = 0;         // Index to track the buffer position

void enableClocks() {
  BITSET(RCC->APB1ENR1, 28); // Power interface clock enable
  BITSET(RCC->APB1ENR1, 1);  // TIM3 clock enable

  BITSET(RCC->AHB2ENR, 6); // GPIOG clock enable
  BITSET(RCC->AHB2ENR, 4); // GPIOE clock enable
  BITSET(RCC->AHB2ENR, 2); // GPIOC clock enable

  BITSET(RCC->APB1ENR2, 0); // LPUART1 clock enable

  BITSET(RCC->APB2ENR, 12); // SPI1 clock enable
  BITSET(RCC->APB2ENR, 11); // TIM1 clock enable
  BITSET(RCC->APB2ENR, 0);  // SYSCFG clock enable

  BITSET(RCC->CCIPR1, 11); // HSI16 clock for LPUART1
  BITCLEAR(RCC->CCIPR1, 10);

  BITSET(RCC->CFGR, 0); // HSI16 clock for SYSCLK

  BITSET(RCC->CR, 8);   // HSI16 clock enable
  BITCLEAR(RCC->CR, 7); // MSI clock at 4MHz
  BITSET(RCC->CR, 6);
  BITSET(RCC->CR, 5);
  BITCLEAR(RCC->CR, 4);
  BITSET(RCC->CR, 0); // MSI clock enable
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
  BITSET(PWR->CR2, 9);      // Enable power to GPIOG
  BITSET(GPIOG->MODER, 17); // Set GPIOG 8 to AF
  BITCLEAR(GPIOG->MODER, 16);
  BITSET(GPIOG->MODER, 15); // Set GPIOG 7 to AF
  BITCLEAR(GPIOG->MODER, 14);

  BITSET(GPIOG->AFR[1], 3); // Set GPIOG 8 to LPUART1_RX
  BITCLEAR(GPIOG->AFR[1], 2);
  BITCLEAR(GPIOG->AFR[1], 1);
  BITCLEAR(GPIOG->AFR[1], 0);

  BITSET(GPIOG->AFR[0], 31); // Set GPIOG 7 to LPUART1_TX
  BITCLEAR(GPIOG->AFR[0], 30);
  BITCLEAR(GPIOG->AFR[0], 29);
  BITCLEAR(GPIOG->AFR[0], 28);

  BITSET(GPIOE->MODER, 31); // Set GPIOE 15 to AF
  BITCLEAR(GPIOE->MODER, 30);
  BITSET(GPIOE->MODER, 27); // Set GPIOE 13 to AF
  BITCLEAR(GPIOE->MODER, 26);
  BITCLEAR(GPIOE->MODER, 25); // Set GPIOE 12 to output
  BITSET(GPIOE->MODER, 24);
  BITCLEAR(GPIOE->MODER, 23); // Set GPIOE 11 to output
  BITSET(GPIOE->MODER, 22);
  BITCLEAR(GPIOE->MODER, 21); // Set GPIOE 10 to output
  BITSET(GPIOE->MODER, 20);
  BITSET(GPIOE->MODER, 7); // Set GPIOE 3 to AF
  BITCLEAR(GPIOE->MODER, 6);

  BITSET(GPIOE->OSPEEDR, 31); // Set GPIOE 15 to very high speed
  BITSET(GPIOE->OSPEEDR, 30);
  BITSET(GPIOE->OSPEEDR, 27); // Set GPIOE 13 to very high speed
  BITSET(GPIOE->OSPEEDR, 26);

  BITCLEAR(GPIOE->AFR[1], 31); // Set GPIOE 15 to SPI1_MOSI
  BITSET(GPIOE->AFR[1], 30);
  BITCLEAR(GPIOE->AFR[1], 29);
  BITSET(GPIOE->AFR[1], 28);
  BITCLEAR(GPIOE->AFR[1], 23); // Set GPIOE 13 to SPI1_SCK
  BITSET(GPIOE->AFR[1], 22);
  BITCLEAR(GPIOE->AFR[1], 21);
  BITSET(GPIOE->AFR[1], 20);

  BITCLEAR(GPIOE->AFR[0], 15); // Set GPIOE 3 to TIM3_CH1
  BITCLEAR(GPIOE->AFR[0], 14);
  BITSET(GPIOE->AFR[0], 13);
  BITCLEAR(GPIOE->AFR[0], 12);
}

void LPUART1_IRQHandler() {
  // RXNE (Receive not empty)
  if (LPUART1->ISR & (1 << 5)) {
    char received_char = LPUART1->RDR;

    if (bufIndex < MAX_BUF_SIZE - 1) {
      buf[bufIndex++] = received_char;
    }

    if (received_char == '\n') { // end of message
      buf[bufIndex] = '\0';
      flag = 1;
    }
  }
  // TXE (Transmit data register empty)
  else if (LPUART1->ISR & (1 << 7)) {
    if (flag == 0) {
      LPUART1->TDR = 'y';
      flag = 1;
      LPUART1->CR1 &= ~(1 << 7); // disable TXE interrupt
    }
  }
}

// Interrupt service routine for PC13
void EXTI13_IRQHandler() {
  EXTI->RPR1 = (1 << 13);   // Clear interrupt flag for PC13
  LPUART1->CR1 |= (1 << 7); // Enable TX interrupt
}

void initLPUART1() {
  LPUART1->BRR = 35555;    // BAUD rate of 115200 (256 * 16Mhz / 115200)
  BITSET(LPUART1->CR1, 5); // Enable RXFIFO not empty interrupt
  BITSET(LPUART1->CR1, 3); // Enable transmitter
  BITSET(LPUART1->CR1, 2); // Enable receiver
  BITSET(LPUART1->CR1, 0); // Enable LPUART1
  NVIC_SetPriority(LPUART1_IRQn, 0); // Set interrupt priority to 0 (max)
  NVIC_EnableIRQ(LPUART1_IRQn);      // Enable LPUART1 IRQ
}

// Initializes PC13
void BTNinit() {
  RCC->AHB2ENR |= (1 << 2); // Enable GPIOC

  // Set up the mode for button at C13
  bitclear(GPIOC->MODER, 26); // Clear bit 26
  bitclear(GPIOC->MODER, 27); // Clear bit 27

  // Set up PC13
  RCC->APB2ENR |= 1;            // Enable Clock to SYSCFG & EXTI
  EXTI->EXTICR[3] = (0x2) << 8; // Select PC13
  EXTI->RTSR1 |= 1 << 13;       // Trigger on rising edge of PC13
  EXTI->IMR1 |= 1 << 13;        // Interrupt mask disable for PC13

  NVIC_SetPriority(EXTI13_IRQn, 0); // Set priority for PC13
  NVIC_EnableIRQ(EXTI13_IRQn);
}

int main() {
  enableClocks();
  initGPIOs();
  initLPUART1();
  initTimers();
  initSPI1();
  initLCD();
  BTNinit();

  while (1) {
    // For testing in Python
    if (flag == 1) {
      for (int i = 0; i < bufIndex; i++) {
        while (!(LPUART1->ISR & (1 << 7)))
          ; // Wait for TXE
        LPUART1->TDR = buf[i];
      }
      flag = 0;
    }
  }

  return 0;
}
