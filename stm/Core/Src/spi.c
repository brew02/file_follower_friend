#include "spi.h"
#include "bitmacro.h"
#include "stm32l552xx.h"

void initSPI1() {
  BITSET(SPI1->CR1, 9);   // Enable software slave management
  BITSET(SPI1->CR1, 8);   // Internal slave select
  BITCLEAR(SPI1->CR1, 7); // Transmit MSB first
  BITCLEAR(SPI1->CR1, 5); // BAUD rate of 8MHz (f_PCLK / 2)
  BITCLEAR(SPI1->CR1, 4);
  BITCLEAR(SPI1->CR1, 3);
  BITSET(SPI1->CR1, 2);   // Master configuration
  BITCLEAR(SPI1->CR1, 1); // Clock is active high (polarity)
  BITCLEAR(SPI1->CR1, 0); // Data captured on first clock transition (phase)

  BITCLEAR(SPI1->CR2, 11); // Data size set at 8-bit
  BITSET(SPI1->CR2, 10);
  BITSET(SPI1->CR2, 9);
  BITSET(SPI1->CR2, 8);

  BITSET(GPIOE->ODR, 12); // Set chip select high
  BITSET(SPI1->CR1, 6);   // Enable SPI1
}

void writeSPI1(uint8_t val) {
  // Write 8 bits
  *(volatile uint8_t *)(&SPI1->DR) = val;
}
