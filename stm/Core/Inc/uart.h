/**
 * This file exposes the functions for
 * using UART protocols.
 *
 * @author Brodie Abrew & Lucas Berry
 */

#ifndef __UART_H
#define __UART_H

#include <stdint.h>
#include <stdlib.h>

#define MAX_BUF_SIZE (1024) // Max string length

/**
 * Sends the characters over LPUART1.
 *
 * @param chars A null-terminated string of characters to send
 */
void sendLPUART1(const char *chars);

/**
 * Sends the character over LPUART1.
 *
 * @param c A character to send
 */
void sendCharLPUART1(const char c);

/**
 * Receives characters over LPUART1 until the
 * buffer size is reached, or a newline character
 * is found.
 *
 * @param buffer The buffer to receive the characters into
 * @param size The size of the buffer
 *
 * @return The number of characters received
 */
size_t receiveLPUART1(char *buffer, size_t size);

/**
 * Sends the characters over LPUART1, then immediately
 * receives characters over LPUART1 until the buffer size is
 * reached, or a newline character is found.
 *
 * @param chars A null-terminated string of characters to send
 * @param buffer The buffer to receive the characters into
 * @param size The size of the buffer
 *
 * @return The number of characters received
 */
size_t sendAndReceiveLPUART1(const char *chars, char *buffer, size_t size);

/**
 * Initializes LPUART1 on the NUCLEO-L552ZE-Q board.
 */
void initLPUART1();

#endif
