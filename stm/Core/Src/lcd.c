#include "lcd.h"
#include "bitmacro.h"
#include "spi.h"
#include "stm32l552xx.h"
#include "timer.h"

enum ST7735_COMMANDS {
  ST7735_NOP = 0x0,
  ST7735_SWRESET = 0x1,
  ST7735_SLPOUT = 0x11,
  ST7735_NORON = 0x13,
  ST7735_INVOFF = 0x20,
  ST7735_DISPON = 0x29,
  ST7735_CASET = 0x2A,
  ST7735_RASET = 0x2B,
  ST7735_RAMWR = 0x2C,
  ST7735_MADCTL = 0x36,
  ST7735_COLMOD = 0x3A,
  ST7735_FRMCTR1 = 0xB1,
  ST7735_FRMCTR2 = 0xB2,
  ST7735_FRMCTR3 = 0xB3,
  ST7735_INVCTR = 0xB4,
  ST7735_PWCTR1 = 0xC0,
  ST7735_PWCTR2 = 0xC1,
  ST7735_PWCTR3 = 0xC2,
  ST7735_PWCTR4 = 0xC3,
  ST7735_PWCTR5 = 0xC4,
  ST7735_VMCTR1 = 0xC5,
  ST7735_GMCTRP1 = 0xE0,
  ST7735_GMCTRN1 = 0xE1
};

static void setLCDReset() {
  // Set LCD reset low (active low)
  BITCLEAR(GPIOE->ODR, 10);
}

static void unsetLCDReset() {
  // Set LCD reset high
  BITSET(GPIOE->ODR, 10);
}

static void setLCDCS() {
  // Set chip select low (active low)
  BITCLEAR(GPIOE->ODR, 12);
}

static void unsetLCDCS() {
  // Set chip select to high
  BITSET(GPIOE->ODR, 12);
}

static void setLCDCommand() {
  // Set LCD register select low (command)
  BITCLEAR(GPIOE->ODR, 11);
}

static void setLCDData() {
  // Set LCD register select high (data)
  BITSET(GPIOE->ODR, 11);
}

static void sendLCDCommand(uint8_t command) {
  // Wait for SPI1 to not be busy and the transmit
  // buffer to be empty
  while (BITCHECK(SPI1->SR, 7) == 1 || BITCHECK(SPI1->SR, 1) == 0)
    ;

  setLCDCommand();

  writeSPI1(command);

  // Wait for SPI1 to not be busy and the transmit
  // buffer to be empty
  while (BITCHECK(SPI1->SR, 7) == 1 || BITCHECK(SPI1->SR, 1) == 0)
    ;

  setLCDData();
}

static void sendLCDData(uint8_t data) {
  // Wait for SPI1 to not be busy and the transmit
  // buffer to be empty
  while (BITCHECK(SPI1->SR, 7) == 1 || BITCHECK(SPI1->SR, 1) == 0)
    ;

  setLCDData();

  writeSPI1(data);

  // Wait for SPI1 to not be busy and the transmit
  // buffer to be empty
  while (BITCHECK(SPI1->SR, 7) == 1 || BITCHECK(SPI1->SR, 1) == 0)
    ;
}

void initLCD() {
  setLCDCS();
  setLCDData();

  // Reset signal must be set for 10 μs minimum
  setLCDReset();
  delayMS(50);

  // Reset lasts 120 ms max
  unsetLCDReset();
  delayMS(120);

  // Exit sleep mode
  // Must wait 120 ms minimum before sending the next command
  sendLCDCommand(ST7735_SLPOUT);
  delayMS(200);

  sendLCDCommand(0x26);
  sendLCDData(0x4);

  sendLCDCommand(0xC0);
  sendLCDData(0xA);
  sendLCDData(0x0);

  sendLCDCommand(ST7735_COLMOD);
  sendLCDData(0x5);
  delayMS(10);

  sendLCDCommand(ST7735_MADCTL);
  sendLCDData(0x8);

  sendLCDCommand(ST7735_NORON);

  sendLCDCommand(ST7735_CASET);
  sendLCDData(0x0);
  sendLCDData(2);
  sendLCDData(0x0);
  sendLCDData(129);

  sendLCDCommand(ST7735_CASET);
  sendLCDData(0x0);
  sendLCDData(3);
  sendLCDData(0x0);
  sendLCDData(130);

  // Fill the screen with black pixels
  sendLCDCommand(ST7735_RAMWR);

  for (int i = 0; i < 128; ++i) {
    for (int j = 0; j < 128; ++j) {
      sendLCDData(0x0);
      sendLCDData(0xF8);
    }
  }

  delayMS(10);
  sendLCDCommand(ST7735_DISPON);
}
