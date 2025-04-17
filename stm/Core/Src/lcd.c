#include "lcd.h"
#include "spi.h"
#include "stm32l552xx.h"
#include "timer.h"

enum ST7735_COMMANDS {
  ST7735_NOP = 0x0,
  ST7735_SLPOUT = 0x11,
  ST7735_DISPON = 0x29,
  ST7735_CASET = 0x2A,
  ST7735_RASET = 0x2B,
  ST7735_RAMWR = 0x2C,
  ST7735_MADCTL = 0x36,
  ST7735_FRMCTR1 = 0xB1,
  ST7735_PWCTR1 = 0xC0
};

void setLCDCS() {
  // Set chip select low (active low)
  GPIOE->ODR &= ~(1 << 12);
}

void unsetLCDCS() {
  // Set chip select to high
  GPIOE->ODR |= (1 << 12);
}

void setLCDCommand() {
  GPIOE->ODR &= ~(1 << 11); // Set LCD register select low (command)
}

void setLCDData() {
  GPIOE->ODR |= (1 << 11); // Set LCD register select high (data)
}

void sendLCDCommand(uint8_t command) {
  // Wait for SPI1 to not be busy
  while (((SPI1->SR >> 7) & 1) == 1)
    ;

  setLCDCommand();

  writeSPI1(command);

  // Wait for SPI1 to not be busy
  while (((SPI1->SR >> 7) & 1) == 1)
    ;

  setLCDData();
}

void sendLCDData(uint8_t data) {
  // Wait for SPI1 to not be busy
  while (((SPI1->SR >> 7) & 1) == 1)
    ;

  setLCDData();

  writeSPI1(data);

  // Wait for SPI1 to not be busy
  while (((SPI1->SR >> 7) & 1) == 1)
    ;
}

void initLCD() {
  GPIOE->ODR &= ~(1 << 10); // Set LCD reset low (active low)
  delayMS(1);
  GPIOE->ODR |= (1 << 10); // Set LCD reset high
  delayMS(1);

  setLCDCS();

  // LCD sleep out command requires 120 MS timeout
  sendLCDCommand(ST7735_SLPOUT);
  delayMS(120);

  // Configure columns to start at 0 and end at 127
  sendLCDCommand(ST7735_CASET);
  sendLCDData(0);
  sendLCDData(0);
  sendLCDData(0);
  sendLCDData(0x7F);

  // Configure rows to start at 0 and end at 127
  sendLCDCommand(ST7735_RASET);
  sendLCDData(0);
  sendLCDData(0);
  sendLCDData(0);
  sendLCDData(0x7F);

  // Configure memory data access control to default
  sendLCDCommand(ST7735_MADCTL);
  sendLCDData(0);

  // Set the frame rate control
  sendLCDCommand(ST7735_FRMCTR1);
  sendLCDData(0x2);
  sendLCDData(0x35);
  sendLCDData(0x35 + 1);

  // Turn the display on
  sendLCDCommand(ST7735_DISPON);

  unsetLCDCS();
}

void useLCD() {
  setLCDCS();

  sendLCDCommand(ST7735_RAMWR);
  for (int i = 0; i < 500; ++i) {
    sendLCDData(50);
  }

  sendLCDCommand(ST7735_NOP);

  unsetLCDCS();
}
