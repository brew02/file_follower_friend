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
  ST7735_GAMSET = 0x26,
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
  delayMS(150);

  // Exit sleep mode
  // Must wait 120 ms minimum before sending the next command
  sendLCDCommand(ST7735_SLPOUT);
  delayMS(200);

  // Set a gamma curve of 2.2
  sendLCDCommand(ST7735_GAMSET);
  sendLCDData(0x4);

  // Set GVDD to 4.30V
  // Set AVDD to 2.5uA
  sendLCDCommand(ST7735_PWCTR1);
  sendLCDData(0xA);
  sendLCDData(0x0);

  // Use a 16-bit color pixel
  sendLCDCommand(ST7735_COLMOD);
  sendLCDData(0x5);
  delayMS(10);

  // Ensure default memory data access control
  sendLCDCommand(ST7735_MADCTL);
  sendLCDData(0x0);

  // Ensure normal display mode is on
  sendLCDCommand(ST7735_NORON);

  // Columns start at 0 and ends before 129
  sendLCDCommand(ST7735_CASET);
  sendLCDData(0x0);
  sendLCDData(0x0);
  sendLCDData(0x0);
  sendLCDData(CFAF_WIDTH);

  // Rows start at 0 and ends before 129
  sendLCDCommand(ST7735_RASET);
  sendLCDData(0x0);
  sendLCDData(0x0);
  sendLCDData(0x0);
  sendLCDData(CFAF_HEIGHT);

  // Fill the screen with red pixels
  sendLCDCommand(ST7735_RAMWR);

  for (int i = 0; i <= CFAF_WIDTH; ++i) {
    for (int j = 0; j <= CFAF_HEIGHT; ++j) {
      sendLCDData(0x0);
      sendLCDData(0xF8);
    }
  }

  delayMS(10);

  // Turn the display on
  sendLCDCommand(ST7735_DISPON);

  delayMS(10);

  unsetLCDCS();
}
