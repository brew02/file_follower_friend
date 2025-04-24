/**
 * This file contains the main entry-point for
 * the program. It also includes function for
 * initializing clocks, GPIOs, LPUART, and the
 * user button on the NUCLEO-L552ZE-Q board.
 *
 * @author Brodie Abrew & Lucas Berry
 */

#include <stdio.h>
#include <string.h>

#include "bitmacro.h"
#include "buttons.h"
#include "lcd.h"
#include "main.h"
#include "spi.h"
#include "stm32l552xx.h"
#include "timer.h"

#define MAX_BUF_SIZE 1024 // Max string length
int flag = 0;             // Flag for UART checking
char buf[MAX_BUF_SIZE];   // Buffer to store received string
int bufIndex = 0;         // Index to track the buffer position

uint16_t bgColor = 0;
uint16_t textColor = 0;
FFFState state = FFF_MENU;
FFFMenu menu = FFF_ACCESS_DIRS;

/**
 * Enables peripheral clocks on the NUCLEO-L552ZE-Q board.
 */
void enableClocks() {
  BITSET(RCC->APB1ENR1, 28); // Power interface clock enable
  BITSET(RCC->APB1ENR1, 1);  // TIM3 clock enable

  BITSET(RCC->AHB2ENR, 13); // ADC clock enable
  BITSET(RCC->AHB2ENR, 6);  // GPIOG clock enable
  BITSET(RCC->AHB2ENR, 4);  // GPIOE clock enable
  BITSET(RCC->AHB2ENR, 2);  // GPIOC clock enable
  BITSET(RCC->AHB2ENR, 0);  // GPIOA clock enable

  BITSET(RCC->APB1ENR2, 0); // LPUART1 clock enable

  BITSET(RCC->APB2ENR, 12); // SPI1 clock enable
  BITSET(RCC->APB2ENR, 0);  // SYSCFG clock enable

  BITSET(RCC->CCIPR1, 29); // System clock for ADCs
  BITSET(RCC->CCIPR1, 28);
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
  GPIOA 5               <-> J1.2  (Joystick Hor(x))
  GPIOA 4               <-> J3.26 (Joystick Ver(y))
  GPIOE 7               <-> J1.5  (Joystick Select)
  GPIOE 6               <-> J4.32 (Button 2)
  GPIOE 5               <-> J4.33 (Button 1)
  GPIOE 3  (TIM3 PWM)   <-> J4.39 (LCD BACKLIGHT)
*/

/**
 * Initializes GPIOs G, E, C, and A on the NUCLEO-L552ZE-Q board.
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
  BITCLEAR(GPIOE->MODER, 15); // Set GPIOE 7 to input
  BITCLEAR(GPIOE->MODER, 14);
  BITCLEAR(GPIOE->MODER, 13); // Set GPIOE 6 to input
  BITCLEAR(GPIOE->MODER, 12);
  BITCLEAR(GPIOE->MODER, 11); // Set GPIOE 5 to input
  BITCLEAR(GPIOE->MODER, 10);
  BITSET(GPIOE->MODER, 7); // Set GPIOE 3 to AF
  BITCLEAR(GPIOE->MODER, 6);

  BITSET(GPIOE->OSPEEDR, 31); // Set GPIOE 15 to very high speed
  BITSET(GPIOE->OSPEEDR, 30);
  BITSET(GPIOE->OSPEEDR, 27); // Set GPIOE 13 to very high speed
  BITSET(GPIOE->OSPEEDR, 26);

  BITCLEAR(GPIOE->OTYPER, 7); // Set GPIOE 7 to push-pull
  BITCLEAR(GPIOE->OTYPER, 6); // Set GPIOE 6 to push-pull
  BITCLEAR(GPIOE->OTYPER, 5); // Set GPIOE 5 to push-pull

  BITSET(GPIOE->PUPDR, 15); // Set GPIOE 7 to pull-down
  BITCLEAR(GPIOE->PUPDR, 14);
  BITSET(GPIOE->PUPDR, 13); // Set GPIOE 6 to pull-down
  BITCLEAR(GPIOE->PUPDR, 12);
  BITSET(GPIOE->PUPDR, 11); // Set GPIOE 5 to pull-down
  BITCLEAR(GPIOE->PUPDR, 10);

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

  BITCLEAR(GPIOC->MODER, 27); // Set GPIOC 13 to input
  BITCLEAR(GPIOC->MODER, 26);

  BITSET(GPIOA->MODER, 11); // Set GPIOA 5 to analog input
  BITSET(GPIOA->MODER, 10);

  BITSET(GPIOA->MODER, 9); // Set GPIOA 4 to analog input
  BITSET(GPIOA->MODER, 8);
}

/**
 * Handles the LPUART1 interrupt (Note:
 * It must have this exact name).
 */
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

/**
 * Initializes LPUART1 on the NUCLEO-L552ZE-Q board.
 */
void initLPUART1() {
  LPUART1->BRR = 35555;    // BAUD rate of 115200 (256 * 16Mhz / 115200)
  BITSET(LPUART1->CR1, 5); // Enable RXFIFO not empty interrupt
  BITSET(LPUART1->CR1, 3); // Enable transmitter
  BITSET(LPUART1->CR1, 2); // Enable receiver
  BITSET(LPUART1->CR1, 0); // Enable LPUART1
  NVIC_SetPriority(LPUART1_IRQn, 0); // Set interrupt priority to 0 (max)
  NVIC_EnableIRQ(LPUART1_IRQn);      // Enable LPUART1 IRQ
}

void initADC1() {
  // Check if the ADC is enabled
  if (BITCHECK(ADC1->CR, 0) == 1) {
    BITSET(ADC1->CR, 1); // Disable ADC1
    // Wait for the ADC to disable
    while (BITCHECK(ADC1->CR, 0) == 1)
      ;
  }

  BITCLEAR(ADC1->CR, 29); // Disable ADC deep-power down
  BITSET(ADC1->CR, 28);   // Enable ADC voltage regulator

  delayMS(10);

  BITSET(ADC12_COMMON->CCR, 21); // Divide ADC clocks by 32
  BITCLEAR(ADC12_COMMON->CCR, 20);
  BITCLEAR(ADC12_COMMON->CCR, 19);
  BITCLEAR(ADC12_COMMON->CCR, 18);

  BITSET(ADC1->ISR, 0); // Clear ADRDY flag

  BITSET(ADC1->CR, 0);                // Enable ADC1
  while (BITCHECK(ADC1->ISR, 0) == 0) // Wait for ADC1 to be ready
    ;

  BITSET(ADC1->ISR, 0); // Clear ADRDY flag

  BITCLEAR(ADC1->JSQR, 18); // Read channel 10 (GPIOA 5)
  BITSET(ADC1->JSQR, 17);
  BITCLEAR(ADC1->JSQR, 16);
  BITSET(ADC1->JSQR, 15);
  BITCLEAR(ADC1->JSQR, 14);

  BITCLEAR(ADC1->JSQR, 12); // Read channel 9 (GPIOA 4)
  BITSET(ADC1->JSQR, 11);
  BITCLEAR(ADC1->JSQR, 10);
  BITCLEAR(ADC1->JSQR, 9);
  BITSET(ADC1->JSQR, 8);

  BITCLEAR(ADC1->JSQR, 1); // Read two injected channels
  BITSET(ADC1->JSQR, 0);
}

/**
 * The main entry-point for the micro-controller.
 */
int main() {
  // Black
  bgColor = color24to16(0x0, 0x0, 0x0);
  // White
  textColor = color24to16(0xFF, 0xFF, 0xFF);

  enableClocks();
  initGPIOs();
  initLPUART1();
  initTimers(brightness);
  initSPI1();
  initLCD(bgColor);
  initButtons();
  initADC1();

  typedef struct {
    uint16_t vert;
    uint16_t horz;
  } JOYSTICK;

  JOYSTICK joystick;
  while (1) {
    //  For testing in Python
    //     if (flag == 1) {
    //       for (int i = 0; i < bufIndex; i++) {
    //         while (!(LPUART1->ISR & (1 << 7)))
    //           ; // Wait for TXE
    //         LPUART1->TDR = buf[i];
    //       }
    //       flag = 0;
    //     }
    if (flag == 1) {
      // renderString(0, 0, buf, text, bg);
      flag = 0;
    }

    BITSET(ADC1->ISR, 6); // Clear the end of injected channel sequence flag
    BITSET(ADC1->CR, 3);  // Start an injected conversion
    // Wait for the injected sequence to finish
    while (BITCHECK(ADC1->ISR, 6) == 0)
      ;
    joystick.vert = ADC1->JDR1 & 0xFFF;
    joystick.horz = ADC1->JDR2 & 0xFFF;

    if (joystick.vert >= 2400 && joystick.vert <= 2700) {
      // This is considered "up" on the joystick
      if (state == FFF_MENU) {
        if (menu != FFF_ACCESS_DIRS) {
          --menu;
        }
      }
    } else if (joystick.vert < 2000) {
      // This is considered "down" on the joystick
      if (state == FFF_MENU) {
        if (menu != FFF_UPD_BRIGHT) {
          ++menu;
        }
      }
    }

    renderMenu();

    if (topButton || bottomButton) {
      if (state == FFF_MENU && menu == FFF_UPD_BRIGHT) {
        if (topButton && brightness <= 95) {
          brightness += 5;
          updateTIM3PWM(brightness);
        }

        if (bottomButton && brightness >= 5) {
          brightness -= 5;
          updateTIM3PWM(brightness);
        }
      }

      topButton = 0;
      bottomButton = 0;
    }

    delayMS(100);
  }

  return 0;
}
