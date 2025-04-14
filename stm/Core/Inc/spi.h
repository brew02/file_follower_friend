#ifndef __SPI_H
#define __SPI_H

#include <stdint.h>

void initSPI1();
uint8_t readSPI1();
void writeSPI1(uint8_t val);

#endif