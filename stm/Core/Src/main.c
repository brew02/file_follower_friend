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
#include "joystick.h"
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
  BITSET(RCC->APB1ENR1, 0);  // TIM2 clock enable

  BITSET(RCC->AHB1ENR, 2); // DMAMUX1 clock enable
  BITSET(RCC->AHB1ENR, 0); // DMA1 clock enable

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

const char *getDirectory(int dir, const char *dirs) {
  int count = 0;
  if (count < 0 || count > LIMITY) {
    return NULL;
  }

  // While we have directory content
  while (*dirs) {
    if (dir == count) {
      return dirs;
    }

    // New directory starts
    if (*dirs++ == '\n') {
      ++count;
    }
  }

  return NULL;
}

int getDirectoryLength(char *dir) {
  int len = 0;
  while (*dir && *dir != '\n') {
    ++len;
    ++dir;
  }

  return len;
}

/**
 * Gets the root directory from the python app.
 *
 * @param buffer The buffer to store the root
 * directory contents in
 * @param size The size that can be received
 *
 * @return The number of bytes received
 */
int rootDir(char *buffer, int size) {
  sendLPUART1("y\n", '\0');

  char cmd[2] = {0};
  receiveLPUART1(cmd, sizeof(cmd), 0);
  if (cmd[0] != 'd' || cmd[1] != ':')
    return 0;

  return receiveLPUART1(buffer, size, 1);
}

typedef enum { TYPE_INVALID = -1, TYPE_DIR, TYPE_IMG, TYPE_FILE } OPEN_TYPE;

/**
 * Opens a path from the python app.
 *
 * @param path The path to open, relative to
 * the current directory
 * @param buffer The buffer to store the newly
 * opened contents in
 * @param type Denotes the type of path that was
 * opened
 * @param size The size that can be received
 *
 * @return The number of bytes received
 */
int openPath(char *path, char *buffer, OPEN_TYPE *type, int size) {
  sendLPUART1("o:", '\0');
  sendLPUART1(path, '\n');
  sendCharLPUART1('\n');

  char cmd[2] = {0};
  receiveLPUART1(cmd, sizeof(cmd), 0);

  if (cmd[0] == 'd' && cmd[1] == ':') {
    // We opened a directory
    *type = TYPE_DIR;
    return receiveLPUART1(buffer, size, 1);
  } else if (cmd[0] == 'b' && cmd[1] == ':') {
    // We opened an image
    *type = TYPE_IMG;
    receiveLPUART1((char *)&size, sizeof(size), 0);
    return receiveLPUART1(buffer, size, 0);
  } else if (cmd[0] == 'f' && cmd[1] == ':') {
    // We opened a file
    *type = TYPE_FILE;
    receiveLPUART1((char *)&size, sizeof(size), 0);
    int len = receiveLPUART1(buffer, size, 0);

    if (len != 0)
      buffer[len - 1] = '\0';

    return len;
  }

  // We opened... nothing
  return 0;
}

/**
 * Gets the parent directory from the python app,
 * relative to the current directory.
 *
 * @param buffer The buffer to store the parent
 * directory contents in
 * @param size The size that can be received
 *
 * @return The number of bytes received
 */
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
 * @param ctx The File Follower Friend context
 */
void handleFriend(FFFContext *ctx) {
  static char buffer[((CFAF_HEIGHT + 1) * (CFAF_WIDTH + 1) * 2) + 1];
  static int len = 0;
  static OPEN_TYPE type = TYPE_INVALID;

  if (ctx->buttons.top) {
    ctx->buttons.top = 0;
    if (ctx->state == STATE_MENU) {
      if (ctx->menuState == MENU_ACCESS_DIRS) {

        // Selected the "Directories" menu option
        int cnt = rootDir(buffer, sizeof(buffer));
        if (cnt == 0)
          return;

        type = TYPE_DIR;
        len = cnt;
        ctx->state = STATE_DIRS;
        ctx->render = 1;
      } else if (ctx->menuState == MENU_UPD_BRIGHT) {
        // Increase LCD backlight brightness (capped at 100)
        int brightness = ctx->brightness + 5;
        ctx->brightness = (brightness > 100 ? 100 : brightness);
        updateTIM3PWM(ctx->brightness);
        ctx->render = 1;
      }
    } else if (ctx->state == STATE_DIRS && type == TYPE_DIR) {
      // Access a path within the current directory
      char *path = (char *)getDirectory(ctx->currentY, buffer);
      if (path != NULL) {
        int cnt = openPath(path, buffer, &type, sizeof(buffer));
        if (cnt == 0)
          return;

        len = cnt;
        ctx->currentY = 0;
        ctx->currentX = 0;
        ctx->render = 1;
      }
    }
  } else if (ctx->buttons.bottom) {
    ctx->buttons.bottom = 0;

    if (ctx->state == STATE_MENU) {
      if (ctx->menuState == MENU_UPD_BRIGHT) {
        // Decrease the LCD backlight brightness (minimum of 0)
        int brightness = ctx->brightness - 5;
        ctx->brightness = (brightness < 0 ? 0 : brightness);
        updateTIM3PWM(ctx->brightness);
        ctx->render = 1;
      }
    } else if (ctx->state == STATE_DIRS) {
      // Access the parent directory of the current directory
      int cnt = parentDir(buffer, sizeof(buffer));
      if (cnt == 0)
        return;

      type = TYPE_DIR;
      len = cnt;
      ctx->currentY = 0;
      ctx->currentX = 0;
      ctx->render = 1;
    }
  } else if (ctx->buttons.joystick) {
    // Return back to the menu
    ctx->buttons.joystick = 0;
    ctx->state = STATE_MENU;
    ctx->menuState = MENU_ACCESS_DIRS;
    type = TYPE_INVALID;
    len = 0;
    ctx->currentY = 0;
    ctx->currentX = 0;
    ctx->render = 1;
  }

  if (ctx->state == STATE_DIRS) {
    if (ctx->render) {
      if (len == 0)
        return;

      // Refresh the screen (can be made more efficient)
      renderFilledRectangle(0, 0, CFAF_WIDTH, CFAF_HEIGHT, ctx->colors.bg);
      if (type == TYPE_DIR) {
        // Render all directories
        renderDirectories(ctx->currentY, ctx->currentX, buffer,
                          ctx->colors.cursor, ctx->colors.dir, ctx->colors.text,
                          ctx->colors.bg);

      } else if (type == TYPE_IMG) {
        // Render an image
        renderImage(buffer, sizeof(buffer));
      } else if (type == TYPE_FILE) {
        // Render the file (won't have any blue highlighting)
        renderDirectories(ctx->currentY, ctx->currentX, buffer,
                          ctx->colors.cursor, ctx->colors.text,
                          ctx->colors.text, ctx->colors.bg);
      }

      ctx->render = 0;
    }

  } else if (ctx->state == STATE_MENU) {
    if (ctx->render) {
      // Refresh the screen (can be made more efficient)
      renderFilledRectangle(0, 0, CFAF_WIDTH, CFAF_HEIGHT, ctx->colors.bg);
      renderMenu(ctx);
      ctx->render = 0;
    }
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
  initJoystick();

  renderMenu(&gCtx);

  while (1) {
    delayMS(50);
    handleFriend(&gCtx);
  }

  return 0;
}
