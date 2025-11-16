#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>

#include "can.h"
#include "io.h"
#include "log.h"
#include "menu.h"
#include "spi.h"
#include "timer.h"
#include "uart.h"
#include "utils.h"
#include "xmem.h"
#include "controller.h"

#define BAUD 9600

LOG_MODULE_DEFINE("main")

// Device Configs
struct USART_config config = {BAUD, F_CPU};

/* Joystick Samples
 *
 * Idle:
 * x: 160
 * y: 165
 *
 * Min:
 * x: 66
 * y: 72
 *
 * Max:
 * x: 250
 * y: 246
 *
 */
struct io_joystick_device joy = {0, 1, 67, 69, 247, 247};
struct io_oled_device oled = {SSB2};
struct io_avr_device avr = {SSB3};
struct can_device can = {SSE2};

// Data Containers
struct io_joystick_position pos;
struct CAN_frame can_msg = {0xFACB, 0x08, "CAN Dumb", 0, 0};
struct io_avr_buttons btn;
struct CAN_frame dummy_msg;

//extern struct can_device can;

// Define settings sub menu
//static struct menu_item settings_menu[] = {
//    {"Adjust brightness", PAGE_ADJUST_BRIGHTNESS, NULL, 0},
//    {"Calibrate joystick", PAGE_CALIBRATE_JOYSTICK, NULL, 0},
//};

// Define main menu
static struct menu_item main_menu[] = {
    {"Start game", PAGE_PLAY_GAME, NULL, 0},
    //{"High scores", PAGE_HIGH_SCORES, NULL, 0},
    //{"Settings", PAGE_SETTINGS, settings_menu, 2},
};

// Initialize menu_cfg struct with menus defined above
struct menu_cfg menu = {
    .oled = &oled,
    .items = main_menu,
    .length = 1,
    .cursor_pos = 0,
    .current_page = PAGE_WELCOME,
    .root_items = main_menu,
    .root_length = 3,
    .parent_menu = NULL,
    .parent_length = 0,
};

struct control_state ctrl = {0};

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

  status = can_init(&can);
  STATUS_ASSERT(status)

  LOG_INF("---Init complete---")

  while (1) {
    //io_avr_buttons_read(&avr, &btn);        // Read button inputs from IO board
    //io_joystick_read_position(&joy, &pos);  // Read joystick input from IO board
    //menu_handler(&menu, &btn);              // Handle menu based on button inputs
    _delay_ms(50);
	
    // Can message send test
	  //struct CAN_frame msg = {CAN_ID_GAMESTART, 0x08, {1, 2, 3, 4, 5, 6,7,8}, 1, 0};
	  //tx_gamestart(&can);
	  
	// Can message recive test
		struct CAN_frame msg;
		if (can_rxq_pull(&msg)) {
			LOG_INF("Reading adress and length of message")
			for (uint8_t i = 0; i<msg.dlc; i++){
			}
			LOG_INF("Done reading data bytes in message")
		} 
	/*
    static enum page_id last_state = PAGE_WELCOME;
    if (menu.current_page != last_state) {
      if(menu.current_page == PAGE_PLAY_GAME) {
        tx_gamestart(&can);
      }
	
      last_state = menu.current_page;
    }
	*/

    //process_can_frame(&ctrl);
	/*
    if (ctrl.game_over == 1) {
      ctrl.game_over = 0;
      menu.current_page = PAGE_GAME_OVER;
	 
    }*/
  }
  return 0;
}
