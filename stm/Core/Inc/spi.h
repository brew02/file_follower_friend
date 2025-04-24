/**
 * This file exposes the functions for
 * controlling SPI1.
 *
 * @author Brodie Abrew & Lucas Berry
 */

#ifndef __SPI_H
#define __SPI_H

#include <stdint.h>

/**
 * Initializes SPI1.
 */
void initSPI1();

/**
 * Writes a value to SPI1.
 *
 * @param val The value to write
 */
void writeSPI1(uint8_t val);

#endif