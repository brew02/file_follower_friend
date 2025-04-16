#include "lcd.h"
#include "spi.h"
#include "stm32l552xx.h"

void initLCD() {
  GPIOE->ODR |= (1 << 10); // Clear LCD reset
}
