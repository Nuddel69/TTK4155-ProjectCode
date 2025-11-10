#include <stddef.h>
#include <stdio.h>
#include <util/delay.h>

#include "can.h"
#include "io.h"
#include "menu.h"
#include "spi.h"
#include "timer.h"
#include "uart.h"
#include "utils.h"
#include "xmem.h"

#define BAUD 9600

struct USART_config config = {BAUD, F_CPU};

struct io_joystick_device joy = {0, 1, 0, 0};

struct io_joystick_position pos;
struct io_avr_buttons btn;

struct io_oled_device oled = {SSB2};
struct io_avr_device avr = {SSB3};

struct can_device can = {SSE2};

// Define settings sub menu
static struct menu_item settings_menu [] = {
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
  .root_length = 3,
  .parent_menu = NULL,
  .parent_length = 0,
};

//menu.length = sizeof(menu_items) / sizeof(menu_items[0]);

int main() {
  int status = 0;

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

  //status = can_init(&can);
  //STATUS_ASSERT(status)

  printf("\n\r---Init Complete---\n\r");

  io_avr_led_set(&avr, 0x0, 0x00);
  io_avr_led_set(&avr, 0x1, 0x00);
  io_avr_led_set(&avr, 0x2, 0x00);
  io_avr_led_set(&avr, 0x3, 0x00);
  io_avr_led_set(&avr, 0x4, 0x00);
  io_avr_led_set(&avr, 0x5, 0x00);

  while (1) {
    io_avr_buttons_read(&avr, &btn);    // Read button inputs from IO board
    menu_handler(&menu, &btn);          // Handle menu based on button inputs
      
    // Button test
    if (btn.NB) {
      io_avr_led_set(&avr, 0x4, 0x0a);
    } else {
      io_avr_led_set(&avr, 0x4, 0x00);
    }
  }
}
