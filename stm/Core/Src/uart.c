/**
 * This file contains functions to initialize, send,
 * and receive over LPUART1.
 *
 * @author Brodie Abrew & Lucas Berry
 */

#include "uart.h"
#include "bitmacro.h"
#include "main.h"
#include "stm32l552xx.h"
#include "timer.h"

void sendLPUART1(const char *chars) {
  int idx = 0;

  // While we haven't reached the null-terminator, send the chars
  while (chars[idx] != '\0') {
    while (BITCHECK(LPUART1->ISR, 7) == 0)
      ;
    LPUART1->TDR = chars[idx++];
  }
}

void sendCharLPUART1(const char c) {
  while (BITCHECK(LPUART1->ISR, 7) == 0)
    ;
  LPUART1->TDR = c;
}

size_t receiveLPUART1(char *buffer, size_t size) {
  size_t cnt = 0;

  while (cnt < (size - 1)) {
    // Timeout if we haven't received a
    // byte for 33ms.
    setSysTickCountdown(33);
    while (BITCHECK(LPUART1->ISR, 5) == 0) {
      if (BITCHECK(SysTick->CTRL, 16) == 1)
        return 0;
    }

    char received = LPUART1->RDR;

    if (received == '\0') {
      buffer[cnt++] = '\0';
      break;
    } else {
      buffer[cnt++] = received;
    }
  }

  buffer[cnt] = '\0';
  return cnt;
}

size_t sendAndReceiveLPUART1(const char *chars, char *buffer, size_t size) {
  sendLPUART1(chars);
  return receiveLPUART1(buffer, size);
}

void initLPUART1() {
  LPUART1->TDR = 0;        // Clear transmit data register
  LPUART1->RDR = 0;        // Clear receive data register
  LPUART1->BRR = 35555;    // BAUD rate of 115200 (256 * 16Mhz / 115200)
  BITSET(LPUART1->CR1, 3); // Enable transmitter
  BITSET(LPUART1->CR1, 2); // Enable receiver
  BITSET(LPUART1->CR1, 0); // Enable LPUART1
}
