#include "joystick.h"
#include "bitmacro.h"
#include "lcd.h"
#include "main.h"
#include "stm32l552xx.h"
#include "timer.h"

void initJoystick() {
  DMA1->IFCR = 0xF;                // Clear interrupt flags for channel 1
  BITCLEAR(DMA1_Channel1->CCR, 0); // Disable channel 1
  while (BITCHECK(DMA1_Channel1->CCR, 0) ==
         1) // Wait for channel 1 to be disabled
    ;
  DMA1->IFCR = 0xF; // Clear interrupt flags for channel 1

  DMA1_Channel1->CPAR =
      (volatile unsigned long)(&ADC1->DR); // Use ADC1 as the peripheral

  DMA1_Channel1->CM0AR =
      (volatile unsigned long)(&gCtx.joystick); // Use the joystick area as
                                                // memory

  DMA1_Channel1->CNDTR = sizeof(gCtx.joystick) / sizeof(gCtx.joystick.vert);

  BITCLEAR(DMA1_Channel1->CCR, 11); // 16-bit memory size
  BITSET(DMA1_Channel1->CCR, 10);
  BITCLEAR(DMA1_Channel1->CCR, 9); // 16-bit peripheral size
  BITSET(DMA1_Channel1->CCR, 8);
  BITSET(DMA1_Channel1->CCR, 7);   // Enable memory increment mode
  BITCLEAR(DMA1_Channel1->CCR, 6); // Disable peripheral increment mode
  BITSET(DMA1_Channel1->CCR, 5);   // Enable circular mode
  BITCLEAR(DMA1_Channel1->CCR, 4); // Read from peripheral, write to memory

  DMAMUX1_Channel0->CCR = 5; // Use ADC1

  BITSET(DMA1_Channel1->CCR, 0); // Enable channel 1

  // Check if the ADC is enabled
  if (BITCHECK(ADC1->CR, 0) == 1) {
    BITSET(ADC1->CR, 1); // Disable ADC1
    // Wait for the ADC to disable
    while (BITCHECK(ADC1->CR, 0) == 1)
      ;
  }

  BITCLEAR(ADC1->CR, 29); // Disable ADC deep-power down
  BITSET(ADC1->CR, 28);   // Enable ADC voltage regulator

  BITCLEAR(ADC1->CFGR, 11); // External trigger on rising edge
  BITSET(ADC1->CFGR, 10);
  BITSET(ADC1->CFGR, 9); // External event 11 (TIM2_TRGO)
  BITCLEAR(ADC1->CFGR, 8);
  BITSET(ADC1->CFGR, 7);
  BITSET(ADC1->CFGR, 6);

  delayMS(10);

  BITCLEAR(ADC1->CFGR, 13); // Single conversion mode
  BITCLEAR(ADC1->CFGR, 12); // Disable overrun mode
  BITSET(ADC1->CFGR, 1);    // Enable DMA circular mode
  BITSET(ADC1->CFGR, 0);    // Enable DMA

  NVIC_EnableIRQ(ADC1_2_IRQn); // Enable ADC1 IRQ
  BITSET(ADC1->IER, 3);        // Enable end of regular sequence interrupt

  BITSET(ADC1->ISR, 0);               // Clear ADRDY flag
  BITSET(ADC1->CR, 0);                // Enable ADC1
  while (BITCHECK(ADC1->ISR, 0) == 0) // Wait for ADC1 to be ready
    ;

  BITSET(ADC1->ISR, 0); // Clear ADRDY flag

  BITCLEAR(ADC1->SQR1, 16); // Read channel 10 (GPIOA 5)
  BITSET(ADC1->SQR1, 15);
  BITCLEAR(ADC1->SQR1, 14);
  BITSET(ADC1->SQR1, 13);
  BITCLEAR(ADC1->SQR1, 12);
  BITCLEAR(ADC1->SQR1, 10); // Read channel 9 (GPIOA 4)
  BITSET(ADC1->SQR1, 9);
  BITCLEAR(ADC1->SQR1, 8);
  BITCLEAR(ADC1->SQR1, 7);
  BITSET(ADC1->SQR1, 6);
  BITCLEAR(ADC1->SQR1, 3); // Read two regular channels
  BITCLEAR(ADC1->SQR1, 2);
  BITCLEAR(ADC1->SQR1, 1);
  BITSET(ADC1->SQR1, 0);

  BITSET(ADC1->CR, 2);  // Start of regular conversion
  BITSET(TIM2->CR1, 0); // Enable TIM2
}

void ADC1_2_IRQHandler() {
  static int polled = 0;

  // Check if it was the end of a regular sequence
  if (BITCHECK(ADC1->ISR, 3) == 1) {
    DMA1->IFCR = 0xF;     // Clear interrupt flags for channel 1
    BITSET(ADC1->ISR, 3); // Clear end of regular sequence interrupt flag

    uint16_t vert = gCtx.joystick.vert & 0xFFF;
    uint16_t horz = gCtx.joystick.horz & 0xFFF;

    if (vert >= 1500 && vert <= 2500 && polled == 1) {
      polled = 0;
    } else if (vert >= 3000 && polled != 1) {

      if (gCtx.state == STATE_DIRS && gCtx.currentY != 0) {
        gCtx.currentY--;

      } else if (gCtx.state == STATE_MENU &&
                 gCtx.menuState != MENU_ACCESS_DIRS) {
        gCtx.menuState--;
      }

      gCtx.render = 1;
      polled = 1;
    } else if (vert <= 1000 && polled != 1) {

      if (gCtx.state == STATE_DIRS && gCtx.currentY != LIMITY) {
        gCtx.currentY++;
      } else if (gCtx.state == STATE_MENU &&
                 gCtx.menuState != MENU_UPD_BRIGHT) {
        gCtx.menuState++;
      }

      gCtx.render = 1;
      polled = 1;
    }
  }
}
