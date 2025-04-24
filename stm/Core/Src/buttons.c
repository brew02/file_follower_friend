/**
 * This file contains functions to initialize
 * and handle interrupts for the various buttons.
 *
 * @author Brodie Abrew & Lucas Berry
 */

#include "buttons.h"
#include "bitmacro.h"
#include "lcd.h"
#include "main.h"
#include "stm32l552xx.h"
#include "timer.h"

void initButtons() {
  BITCLEAR(EXTI->EXTICR[1], 31); // Select GPIOE 7
  BITCLEAR(EXTI->EXTICR[1], 30);
  BITCLEAR(EXTI->EXTICR[1], 29);
  BITCLEAR(EXTI->EXTICR[1], 28);
  BITCLEAR(EXTI->EXTICR[1], 27);
  BITSET(EXTI->EXTICR[1], 26);
  BITCLEAR(EXTI->EXTICR[1], 25);
  BITCLEAR(EXTI->EXTICR[1], 24);
  BITSET(EXTI->RTSR1, 7); // Trigger interrupt on external interrupt 7
  BITSET(EXTI->IMR1, 7);  // Disable interrupt mask for external interrupt 7

  NVIC_SetPriority(EXTI7_IRQn, 0); // Set priority for external interrupt 7
  NVIC_EnableIRQ(EXTI7_IRQn);      // Enable external interrupt 7

  BITCLEAR(EXTI->EXTICR[1], 23); // Select GPIOE 6
  BITCLEAR(EXTI->EXTICR[1], 22);
  BITCLEAR(EXTI->EXTICR[1], 21);
  BITCLEAR(EXTI->EXTICR[1], 20);
  BITCLEAR(EXTI->EXTICR[1], 19);
  BITSET(EXTI->EXTICR[1], 18);
  BITCLEAR(EXTI->EXTICR[1], 17);
  BITCLEAR(EXTI->EXTICR[1], 16);
  BITSET(EXTI->RTSR1, 6); // Trigger interrupt on external interrupt 6
  BITSET(EXTI->IMR1, 6);  // Disable interrupt mask for external interrupt 6

  NVIC_SetPriority(EXTI6_IRQn, 0); // Set priority for external interrupt 6
  NVIC_EnableIRQ(EXTI6_IRQn);      // Enable external interrupt 6

  BITCLEAR(EXTI->EXTICR[1], 15); // Select GPIOE 5
  BITCLEAR(EXTI->EXTICR[1], 14);
  BITCLEAR(EXTI->EXTICR[1], 13);
  BITCLEAR(EXTI->EXTICR[1], 12);
  BITCLEAR(EXTI->EXTICR[1], 11);
  BITSET(EXTI->EXTICR[1], 10);
  BITCLEAR(EXTI->EXTICR[1], 9);
  BITCLEAR(EXTI->EXTICR[1], 8);
  BITSET(EXTI->RTSR1, 5); // Trigger interrupt on external interrupt 5
  BITSET(EXTI->IMR1, 5);  // Disable interrupt mask for external interrupt 5

  NVIC_SetPriority(EXTI5_IRQn, 0); // Set priority for external interrupt 5
  NVIC_EnableIRQ(EXTI5_IRQn);      // Enable external interrupt 5

  BITCLEAR(EXTI->EXTICR[3], 15); // Select GPIOC 13
  BITCLEAR(EXTI->EXTICR[3], 14);
  BITCLEAR(EXTI->EXTICR[3], 13);
  BITCLEAR(EXTI->EXTICR[3], 12);
  BITCLEAR(EXTI->EXTICR[3], 11);
  BITCLEAR(EXTI->EXTICR[3], 10);
  BITSET(EXTI->EXTICR[3], 9);
  BITCLEAR(EXTI->EXTICR[3], 8);
  BITSET(EXTI->RTSR1, 13); // Trigger interrupt on external interrupt 13
  BITSET(EXTI->IMR1, 13);  // Disable interrupt mask for external interrupt 13

  NVIC_SetPriority(EXTI13_IRQn, 0); // Set priority for external interrupt 13
  NVIC_EnableIRQ(EXTI13_IRQn);      // Enable external interrupt 13
}

/**
 * Handles external interrupt 13 (Note:
 * It must have this exact name).
 */
void EXTI13_IRQHandler() {
  BITSET(EXTI->RPR1, 13);  // Clear interrupt flag for external interrupt 13
  BITSET(LPUART1->CR1, 7); // Enable TX interrupt
}

/**
 * Handles external interrupt 7 (Note:
 * It must have this exact name).
 */
void EXTI7_IRQHandler() {
  // This is the joystick push button
  BITSET(EXTI->RPR1, 7); // Clear interrupt flag for external interrupt 7
}

/**
 * Handles external interrupt 6 (Note:
 * It must have this exact name).
 */
void EXTI6_IRQHandler() {
  delayMS(10);
  // This is the bottom button on the BOOSTXL-EDUMKII
  if (state == FFF_MENU && menu == FFF_UPD_BRIGHT && brightness >= 5) {
    brightness -= 5;
    updateTIM3PWM(brightness);
  }
  renderChar(CFAF_WIDTH - 5, CFAF_HEIGHT - 7, 'A', textColor, bgColor);
  BITSET(EXTI->RPR1, 6); // Clear interrupt flag for external interrupt 6
}

/**
 * Handles external interrupt 5 (Note:
 * It must have this exact name).
 */
void EXTI5_IRQHandler() {
  delayMS(10);
  // This is the top button on the BOOSTXL-EDUMKII
  if (state == FFF_MENU && menu == FFF_UPD_BRIGHT && brightness <= 95) {
    brightness += 5;
    updateTIM3PWM(brightness);
  }
  renderChar(CFAF_WIDTH - 5, CFAF_HEIGHT - 7, 'B', textColor, bgColor);
  BITSET(EXTI->RPR1, 5); // Clear interrupt flag for external interrupt 5
}
