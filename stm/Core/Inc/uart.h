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

/**
 * Sends the characters over LPUART1.
 *
 * @param chars An array of characters to send
 * @param end The character to stop sending at
 */
void sendLPUART1(const char *chars, const char end);

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
 * @param terminate 1 if the buffer should be null-terminated,
 * 0 if not
 *
 * @return The number of characters received
 */
int receiveLPUART1(char *buffer, int size, int terminate);

/**
 * Sends the characters over LPUART1, then immediately
 * receives characters over LPUART1 until the buffer size is
 * reached, or a newline character is found.
 *
 * @param chars An array of characters to send
 * @param end The character to stop sending at
 * @param buffer The buffer to receive the characters into
 * @param size The size of the buffer
 * @param terminate 1 if the buffer should be null-terminated,
 * 0 if not
 *
 * @return The number of characters received
 */
int sendAndReceiveLPUART1(const char *chars, const char end, char *buffer,
                          int size, int terminate);

/**
 * Initializes LPUART1 on the NUCLEO-L552ZE-Q board.
 */
void initLPUART1();

#endif
