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

void sendLPUART1(const char *chars, const char end) {
  int idx = 0;

  // While we haven't reached the end char, send the chars
  while (chars[idx] != end) {
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

int receiveLPUART1(char *buffer, int size, int string) {
  int cnt = 0;

  while (cnt < size) {
    while (BITCHECK(LPUART1->ISR, 5) == 0)
      ;

    char received = LPUART1->RDR;
    buffer[cnt++] = received;

    if (string == 1 && received == '\0')
      break;
  }

  if (string == 1)
    buffer[cnt] = '\0';

  return cnt;
}

int sendAndReceiveLPUART1(const char *chars, char end, char *buffer, int size,
                          int string) {
  sendLPUART1(chars, end);
  return receiveLPUART1(buffer, size, string);
}

void initLPUART1() {
  LPUART1->BRR = 35555;    // BAUD rate of 115200 (256 * 16Mhz / 115200)
  BITSET(LPUART1->CR1, 3); // Enable transmitter
  BITSET(LPUART1->CR1, 2); // Enable receiver
  BITSET(LPUART1->CR1, 0); // Enable LPUART1
}
