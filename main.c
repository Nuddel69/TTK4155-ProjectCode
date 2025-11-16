/**
 * @file main.c
 * @brief Main entry point for the TTK4155 Node 1 firmware
 *
 * This file initializes all peripherals and modules, then enters the main
 * game loop. The loop handles:
 *  - Reading input from the I/O board
 *  - Sending and receiving state-information over CAN
 *  - Updating menu states and page transitions
 *  - Managing game start and game over conditions
 *
 * Modules used:
 *  - IO
 *  - USART
 *  - SPI
 *  - TIMER
 *  - CAN
 *  - CONTROLLER
 *  - MENU
 *  - XMEM
 *  - UTILS
 *  - LOG
 *
 * @ingroup Application
 */

#include <stddef.h>
#include <stdlib.h>
#include <util/delay.h>

#include "can.h"
#include "controller.h"
#include "io.h"
#include "log.h"
#include "menu.h"
#include "spi.h"
#include "timer.h"
#include "uart.h"
#include "utils.h"
#include "xmem.h"

#define BAUD 9600

LOG_MODULE_DEFINE("main");

// ---------------------------------------------------------------------------
// Device Configs
// ---------------------------------------------------------------------------
struct USART_config config = {BAUD, F_CPU};
struct io_joystick_device joy = {0, 1, 67, 69, 247, 247};
struct io_oled_device oled = {SSB2};
struct io_avr_device avr = {SSB3};
struct can_device can = {SSE2};

// ---------------------------------------------------------------------------
// Data Containers
// ---------------------------------------------------------------------------
struct io_joystick_position pos;
struct io_avr_buttons btn;
struct CAN_frame dummy_msg;

// Define settings sub menu
static struct menu_item settings_menu[] = {
    {"Adjust brightness", PAGE_ADJUST_BRIGHTNESS, NULL, 0},
    {"Calibrate joystick", PAGE_CALIBRATE_JOYSTICK, NULL, 0},
};

// Define main menu
static struct menu_item main_menu[] = {
    {"Start game", PAGE_PLAY_GAME, NULL, 0},
    {"High scores", PAGE_HIGH_SCORES, NULL, 0},
    {"Settings", PAGE_SETTINGS, settings_menu, 2},
};

// Initialize menu_cfg struct with menus defined above
struct menu_cfg menu = {
    .oled = &oled,
    .items = main_menu,
    .length = 3,
    .cursor_pos = 0,
    .current_page = PAGE_WELCOME,
    .root_items = main_menu,
    .root_length = 1,
    .parent_menu = NULL,
    .parent_length = 0,
};

// ---------------------------------------------------------------------------
// Game state
// ---------------------------------------------------------------------------
struct control_state ctrl = {0};
static enum page_id last_state = PAGE_WELCOME;

// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------
int main() {
  int status = 0;

  // ---------------------------------------------------------------------------
  // Initializations
  // ---------------------------------------------------------------------------

  status = USART_init(&config);
  STATUS_ASSERT(status);

  status = xmem_init();
  STATUS_ASSERT(status)

  status = SRAM_test();
  STATUS_ASSERT(status)

  status = tim1_CTC_init();
  STATUS_ASSERT(status)

  status = io_avr_init(&avr);
  STATUS_ASSERT(status)

  status = menu_init(&menu);
  STATUS_ASSERT(status)

  status = can_init(&can);
  STATUS_ASSERT(status)

  LOG_CRITICAL("---Init complete---");

  static enum page_id last_state = PAGE_WELCOME;

  while (1) {

    // ---------------------------------------------------------------------------
    // Input/Output Control
    // ---------------------------------------------------------------------------

    io_avr_buttons_read(&avr, &btn); // Read button inputs from IO board
    process_can_frame(&ctrl);        // Parse CAN

    // ---------------------------------------------------------------------------
    // Game Control
    // ---------------------------------------------------------------------------

    menu_handler(&menu, &btn);             // Handle menu state
    if (menu.current_page != last_state) { // Check for game start
      if (menu.current_page == PAGE_PLAY_GAME) {
        tx_gamestart(&can);
      }
      last_state = menu.current_page;
    } else {
      tx_joy_btn(&joy, &avr, &can); // Transmit input states over CAN
    }

    if (ctrl.game_over) { // State game-over
      ctrl.game_over = 0;
      if (menu.current_page == PAGE_PLAY_GAME) {
        LOG_CRITICAL("Game over");
        menu.current_page = PAGE_GAME_OVER;
      }
    }

    _delay_ms(100);
  }
  return 0;
}

/** @} */
