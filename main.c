#include <stddef.h>
#include <stdio.h>
#include <util/delay.h>

#include "io.h"
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
struct io_avr_device avr = {SSE2};

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

  printf("\n\r---Init Complete---\n\r");

  io_avr_led_set(&avr, 0x0, 0x00);
  io_avr_led_set(&avr, 0x1, 0x00);
  io_avr_led_set(&avr, 0x2, 0x00);
  io_avr_led_set(&avr, 0x3, 0x00);
  io_avr_led_set(&avr, 0x4, 0x00);
  io_avr_led_set(&avr, 0x5, 0x00);

  while (1) {
    io_avr_buttons_read(&avr, &btn);

    if (btn.NB) {
      io_avr_led_set(&avr, 0x4, 0x0a);
    } else {
      io_avr_led_set(&avr, 0x4, 0x00);
    }
  }
}
