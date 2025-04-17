#include "spi.h"
#include "stm32l552xx.h"

void initSPI1() {
  SPI1->CR1 |= (1 << 9) |     // Enable software slave management
               (1 << 8) |     // Internal slave select
               (0b001 << 3) | // BAUD rate of 4MHz (f_PCLK / 4)
               (1 << 2);      // Master configuration

  SPI1->CR2 |= (0b0111 << 8); // Data size set at 8-bit

  GPIOE->ODR |= (1 << 12); // Set chip select high
  SPI1->CR1 |= (1 << 6);   // Enable SPI1
}

void writeSPI1(uint8_t val) {
  // Write 8 bits
  *(volatile uint8_t *)(&SPI1->DR) = val;
}
