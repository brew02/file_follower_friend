/**
 * This file contains the main entry-point for
 * the program. It also includes function for
 * initializing clocks, GPIOs, LPUART, and the
 * user button on the NUCLEO-L552ZE-Q board.
 *
 * @author Brodie Abrew & Lucas Berry
 */

#include <stdio.h>
#include <string.h>

#include "bitmacro.h"
#include "buttons.h"
#include "lcd.h"
#include "main.h"
#include "spi.h"
#include "stm32l552xx.h"
#include "timer.h"
#include "uart.h"

FFFContext gCtx = {0};

/**
 * Enables peripheral clocks on the NUCLEO-L552ZE-Q board.
 */
void enableClocks() {
  BITSET(RCC->APB1ENR1, 28); // Power interface clock enable
  BITSET(RCC->APB1ENR1, 1);  // TIM3 clock enable

  BITSET(RCC->AHB2ENR, 13); // ADC clock enable
  BITSET(RCC->AHB2ENR, 6);  // GPIOG clock enable
  BITSET(RCC->AHB2ENR, 4);  // GPIOE clock enable
  BITSET(RCC->AHB2ENR, 2);  // GPIOC clock enable
  BITSET(RCC->AHB2ENR, 0);  // GPIOA clock enable

  BITSET(RCC->APB1ENR2, 0); // LPUART1 clock enable

  BITSET(RCC->APB2ENR, 12); // SPI1 clock enable
  BITSET(RCC->APB2ENR, 0);  // SYSCFG clock enable

  BITSET(RCC->CCIPR1, 29); // System clock for ADCs
  BITSET(RCC->CCIPR1, 28);
  BITSET(RCC->CCIPR1, 11); // HSI16 clock for LPUART1
  BITCLEAR(RCC->CCIPR1, 10);

  BITSET(RCC->CFGR, 0); // HSI16 clock for SYSCLK

  BITSET(RCC->CR, 8);   // HSI16 clock enable
  BITCLEAR(RCC->CR, 7); // MSI clock at 4MHz
  BITSET(RCC->CR, 6);
  BITSET(RCC->CR, 5);
  BITCLEAR(RCC->CR, 4);
  BITSET(RCC->CR, 0); // MSI clock enable
}

/*
  GPIOE 15 (SPI1_MOSI)  <-> J2.15 (LCD SDA/SPI MOSI)
  GPIOE 13 (SPI1_SCK)   <-> J1.7 (LCD SPI CLK)
  GPIOE 12              <-> J2.13 (LCD SPI CS)
  GPIOE 11              <-> J4.31 (LCD RS)
  GPIOE 10              <-> J2.17 (LCD RST)
  GPIOA 5               <-> J1.2  (Joystick Hor(x))
  GPIOA 4               <-> J3.26 (Joystick Ver(y))
  GPIOE 7               <-> J1.5  (Joystick Select)
  GPIOE 6               <-> J4.32 (Button 2)
  GPIOE 5               <-> J4.33 (Button 1)
  GPIOE 3  (TIM3 PWM)   <-> J4.39 (LCD BACKLIGHT)
*/

/**
 * Initializes GPIOs G, E, C, and A on the NUCLEO-L552ZE-Q board.
 */
void initGPIOs() {
  BITSET(PWR->CR2, 9);      // Enable power to GPIOG
  BITSET(GPIOG->MODER, 17); // Set GPIOG 8 to AF
  BITCLEAR(GPIOG->MODER, 16);
  BITSET(GPIOG->MODER, 15); // Set GPIOG 7 to AF
  BITCLEAR(GPIOG->MODER, 14);

  BITSET(GPIOG->AFR[1], 3); // Set GPIOG 8 to LPUART1_RX
  BITCLEAR(GPIOG->AFR[1], 2);
  BITCLEAR(GPIOG->AFR[1], 1);
  BITCLEAR(GPIOG->AFR[1], 0);

  BITSET(GPIOG->AFR[0], 31); // Set GPIOG 7 to LPUART1_TX
  BITCLEAR(GPIOG->AFR[0], 30);
  BITCLEAR(GPIOG->AFR[0], 29);
  BITCLEAR(GPIOG->AFR[0], 28);

  BITSET(GPIOE->MODER, 31); // Set GPIOE 15 to AF
  BITCLEAR(GPIOE->MODER, 30);
  BITSET(GPIOE->MODER, 27); // Set GPIOE 13 to AF
  BITCLEAR(GPIOE->MODER, 26);
  BITCLEAR(GPIOE->MODER, 25); // Set GPIOE 12 to output
  BITSET(GPIOE->MODER, 24);
  BITCLEAR(GPIOE->MODER, 23); // Set GPIOE 11 to output
  BITSET(GPIOE->MODER, 22);
  BITCLEAR(GPIOE->MODER, 21); // Set GPIOE 10 to output
  BITSET(GPIOE->MODER, 20);
  BITCLEAR(GPIOE->MODER, 15); // Set GPIOE 7 to input
  BITCLEAR(GPIOE->MODER, 14);
  BITCLEAR(GPIOE->MODER, 13); // Set GPIOE 6 to input
  BITCLEAR(GPIOE->MODER, 12);
  BITCLEAR(GPIOE->MODER, 11); // Set GPIOE 5 to input
  BITCLEAR(GPIOE->MODER, 10);
  BITSET(GPIOE->MODER, 7); // Set GPIOE 3 to AF
  BITCLEAR(GPIOE->MODER, 6);

  BITSET(GPIOE->OSPEEDR, 31); // Set GPIOE 15 to very high speed
  BITSET(GPIOE->OSPEEDR, 30);
  BITSET(GPIOE->OSPEEDR, 27); // Set GPIOE 13 to very high speed
  BITSET(GPIOE->OSPEEDR, 26);

  BITCLEAR(GPIOE->OTYPER, 7); // Set GPIOE 7 to push-pull
  BITCLEAR(GPIOE->OTYPER, 6); // Set GPIOE 6 to push-pull
  BITCLEAR(GPIOE->OTYPER, 5); // Set GPIOE 5 to push-pull

  BITSET(GPIOE->PUPDR, 15); // Set GPIOE 7 to pull-down
  BITCLEAR(GPIOE->PUPDR, 14);
  BITSET(GPIOE->PUPDR, 13); // Set GPIOE 6 to pull-down
  BITCLEAR(GPIOE->PUPDR, 12);
  BITSET(GPIOE->PUPDR, 11); // Set GPIOE 5 to pull-down
  BITCLEAR(GPIOE->PUPDR, 10);

  BITCLEAR(GPIOE->AFR[1], 31); // Set GPIOE 15 to SPI1_MOSI
  BITSET(GPIOE->AFR[1], 30);
  BITCLEAR(GPIOE->AFR[1], 29);
  BITSET(GPIOE->AFR[1], 28);
  BITCLEAR(GPIOE->AFR[1], 23); // Set GPIOE 13 to SPI1_SCK
  BITSET(GPIOE->AFR[1], 22);
  BITCLEAR(GPIOE->AFR[1], 21);
  BITSET(GPIOE->AFR[1], 20);

  BITCLEAR(GPIOE->AFR[0], 15); // Set GPIOE 3 to TIM3_CH1
  BITCLEAR(GPIOE->AFR[0], 14);
  BITSET(GPIOE->AFR[0], 13);
  BITCLEAR(GPIOE->AFR[0], 12);

  BITCLEAR(GPIOC->MODER, 27); // Set GPIOC 13 to input
  BITCLEAR(GPIOC->MODER, 26);

  BITSET(GPIOA->MODER, 11); // Set GPIOA 5 to analog input
  BITSET(GPIOA->MODER, 10);

  BITSET(GPIOA->MODER, 9); // Set GPIOA 4 to analog input
  BITSET(GPIOA->MODER, 8);
}

void initADC1() {
  // Check if the ADC is enabled
  if (BITCHECK(ADC1->CR, 0) == 1) {
    BITSET(ADC1->CR, 1); // Disable ADC1
    // Wait for the ADC to disable
    while (BITCHECK(ADC1->CR, 0) == 1)
      ;
  }

  BITCLEAR(ADC1->CR, 29); // Disable ADC deep-power down
  BITSET(ADC1->CR, 28);   // Enable ADC voltage regulator

  delayMS(10);

  BITSET(ADC12_COMMON->CCR, 21); // Divide ADC clocks by 32
  BITCLEAR(ADC12_COMMON->CCR, 20);
  BITCLEAR(ADC12_COMMON->CCR, 19);
  BITCLEAR(ADC12_COMMON->CCR, 18);

  BITSET(ADC1->ISR, 0); // Clear ADRDY flag

  BITSET(ADC1->CR, 0);                // Enable ADC1
  while (BITCHECK(ADC1->ISR, 0) == 0) // Wait for ADC1 to be ready
    ;

  BITSET(ADC1->ISR, 0); // Clear ADRDY flag

  BITCLEAR(ADC1->JSQR, 18); // Read channel 10 (GPIOA 5)
  BITSET(ADC1->JSQR, 17);
  BITCLEAR(ADC1->JSQR, 16);
  BITSET(ADC1->JSQR, 15);
  BITCLEAR(ADC1->JSQR, 14);

  BITCLEAR(ADC1->JSQR, 12); // Read channel 9 (GPIOA 4)
  BITSET(ADC1->JSQR, 11);
  BITCLEAR(ADC1->JSQR, 10);
  BITCLEAR(ADC1->JSQR, 9);
  BITSET(ADC1->JSQR, 8);

  BITCLEAR(ADC1->JSQR, 1); // Read two injected channels
  BITSET(ADC1->JSQR, 0);
}

/**
 * Gets the directory from a string containing all
 * content in the current directory.
 *
 * @param dir The directory to get
 * @param dirs The content of the current directory
 *
 * @return NULL if the specified directory is not
 * a directory, a string representing the relative
 * path of the directory otherwise.
 */
char *getDirectory(int dir, char *dirs) {
  int count = 0;
  if (count < 0 || count > LIMITY) {
    return NULL;
  }

  while (*dirs) {
    if (dir == count) {
      return dirs;
    }

    if (*dirs++ == '\n') {
      ++count;
    }
  }

  return NULL;
}

int rootDir(char *buffer, int size) {
  sendLPUART1("y\n", '\0');

  char cmd[2] = {0};
  receiveLPUART1(cmd, sizeof(cmd), 0);
  if (cmd[0] != 'd' || cmd[1] != ':')
    return 0;

  return receiveLPUART1(buffer, size, 1);
}

typedef enum { TYPE_INVALID = -1, TYPE_DIR, TYPE_IMG, TYPE_FILE } OPEN_TYPE;

int openPath(char *path, char *buffer, OPEN_TYPE *type, int size) {
  sendLPUART1("o:", '\0');
  sendLPUART1(path, '\n');
  sendCharLPUART1('\n');

  char cmd[2] = {0};
  receiveLPUART1(cmd, sizeof(cmd), 0);
  if (cmd[0] == 'd' && cmd[1] == ':') {
    *type = TYPE_DIR;
    return receiveLPUART1(buffer, size, 1);
  } else if (cmd[0] == 'b' && cmd[1] == ':') {
    *type = TYPE_IMG;
    receiveLPUART1((char *)&size, sizeof(size), 0);
    return receiveLPUART1(buffer, size, 0);
  }

  return 0;
}

int parentDir(char *buffer, int size) {
  sendLPUART1("g\n", '\0');

  char cmd[2] = {0};
  receiveLPUART1(cmd, sizeof(cmd), 0);
  if (cmd[0] != 'd' || cmd[1] != ':')
    return 0;

  return receiveLPUART1(buffer, size, 1);
}

/**
 * Handles the main functionality of File
 * Follower Friend (FFF).
 *
 * @param joystick Contains the ADC converted
 * vertical and horizontal values of the joystick
 */
void handleFriend(FFFContext *ctx) {
  static char buffer[((CFAF_HEIGHT + 1) * (CFAF_WIDTH + 1) * 2) + 1];
  static int len = 0;
  static int render = 0;
  static int currentY = 0;
  static OPEN_TYPE type = TYPE_INVALID;

  if (ctx->buttons.top) {
    ctx->buttons.top = 0;

    if (ctx->state == STATE_MENU) {
      if (ctx->menuState == MENU_ACCESS_DIRS) {
        int cnt = rootDir(buffer, sizeof(buffer));
        if (cnt == 0)
          return;

        type = TYPE_DIR;
        len = cnt;
        ctx->state = STATE_DIRS;
        render = 1;
      } else if (ctx->menuState == MENU_UPD_BRIGHT) {
        int brightness = ctx->brightness + 5;
        ctx->brightness = (brightness > 100 ? 100 : brightness);
        updateTIM3PWM(ctx->brightness);
        render = 1;
      }
    } else if (ctx->state == STATE_DIRS) {
      char *path = getDirectory(currentY, buffer);
      if (path != NULL) {
        int cnt = openPath(path, buffer, &type, sizeof(buffer));
        if (cnt == 0)
          return;

        len = cnt;
        currentY = 0;
        render = 1;
      }
    }
  } else if (ctx->buttons.bottom) {
    ctx->buttons.bottom = 0;

    if (ctx->state == STATE_MENU) {
      if (ctx->menuState == MENU_UPD_BRIGHT) {
        int brightness = ctx->brightness - 5;
        ctx->brightness = (brightness < 0 ? 0 : brightness);
        updateTIM3PWM(ctx->brightness);
        render = 1;
      }
    } else if (ctx->state == STATE_DIRS) {
      int cnt = parentDir(buffer, sizeof(buffer));
      if (cnt == 0)
        return;

      type = TYPE_DIR;
      len = cnt;
      currentY = 0;
      render = 1;
    }
  } else if (ctx->buttons.joystick) {
    ctx->buttons.joystick = 0;
    ctx->state = STATE_MENU;
    ctx->menuState = MENU_ACCESS_DIRS;
    type = TYPE_INVALID;
    len = 0;
    currentY = 0;
    render = 1;
  }

  if (ctx->state == STATE_DIRS) {
    if (ctx->joystick.vert >= 2400 && ctx->joystick.vert <= 2700 &&
        currentY != 0) {
      // Up on joystick
      --currentY;
      render = 1;
    } else if (ctx->joystick.vert < 2000 && currentY != LIMITY) {
      // Down on joystick
      ++currentY;
      render = 1;
    }

    if (render && len != 0) {
      // Refresh the screen (can be made more efficient)
      renderFilledRectangle(0, 0, CFAF_WIDTH, CFAF_HEIGHT, ctx->colors.bg);
      if (type == TYPE_DIR) {
        renderDirectories(currentY, buffer, ctx->colors.cursor, ctx->colors.dir,
                          ctx->colors.text, ctx->colors.bg);

      } else if (type == TYPE_IMG) {
        renderImage(buffer, sizeof(buffer));
      }
    }

    render = 0;
  } else if (ctx->state == STATE_MENU) {
    if (ctx->joystick.vert >= 2400 && ctx->joystick.vert <= 2700 &&
        ctx->menuState != MENU_ACCESS_DIRS) {
      // Up on joystick
      ctx->menuState--;
      render = 1;
    } else if (ctx->joystick.vert < 2000 && ctx->menuState != MENU_UPD_BRIGHT) {
      // Down on joystick
      ctx->menuState++;
      render = 1;
    }

    if (render) {
      // Refresh the screen (can be made more efficient)
      renderFilledRectangle(0, 0, CFAF_WIDTH, CFAF_HEIGHT, ctx->colors.bg);
      renderMenu(ctx);
    }

    render = 0;
  }
}

/**
 * The main entry-point for the micro-controller.
 */
int main() {
  memset(&gCtx, 0, sizeof(gCtx));
  gCtx.state = STATE_MENU;
  gCtx.menuState = MENU_ACCESS_DIRS;
  // Black
  gCtx.colors.bg = color24to16(0x0, 0x0, 0x0);
  // White
  gCtx.colors.text = color24to16(0xFF, 0xFF, 0xFF);
  // Blue
  gCtx.colors.dir = color24to16(0x7B, 0x74, 0xFF);
  // Green
  gCtx.colors.cursor = color24to16(0x83, 0xFA, 0x89);
  gCtx.brightness = 20;

  enableClocks();
  initGPIOs();
  initLPUART1();
  initTimers(gCtx.brightness);
  initSPI1();
  initLCD(gCtx.colors.bg);
  initButtons();
  initADC1();

  renderMenu(&gCtx);

  while (1) {
    BITSET(ADC1->ISR, 6); // Clear the end of injected channel sequence flag
    BITSET(ADC1->CR, 3);  // Start an injected conversion
    // Wait for the injected sequence to finish
    while (BITCHECK(ADC1->ISR, 6) == 0)
      ;
    gCtx.joystick.vert = ADC1->JDR1 & 0xFFF;
    gCtx.joystick.horz = ADC1->JDR2 & 0xFFF;

    handleFriend(&gCtx);

    delayMS(50);
  }

  return 0;
}
