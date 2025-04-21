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

void writedata(uint8_t c) {
	GPIOE->ODR |= (1 << 11);

	GPIOE->ODR &= ~(1 << 12);  // Set chip select

	// Wait until the TXE flag is set
	while (!(SPI1->SR & (1 << 1)));

	// Send the 8-bit data to the SPI data register
	SPI1->DR = c;

	// Wait until the RXNE (Receive buffer not empty) flag is set
	while (!(SPI1->SR & (1 << 0)));

	// Read the received data to clear the RXNE flag
	uint8_t dummy = SPI1->DR;

	GPIOE->ODR |= (1 << 12);  // deactivate chip select
}
