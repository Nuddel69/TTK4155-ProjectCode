#include <stddef.h>
#include <stdio.h>
#include <util/delay.h>

#include "io.h"
#include "spi.h"
#include "timer.h"
#include "uart.h"
#include "utils.h"
#include "xmem.h"

#define FOSC 4915200
#define BAUD 9600

struct USART_config config = {BAUD, FOSC};

struct io_joystick_device joy = {0, 1, 0, 0};

struct io_joystick_position pos;

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

  status = spi_init(NULL);
  STATUS_ASSERT(status)

  status = io_joystick_init(&joy);
  STATUS_ASSERT(status)

  printf("\n\r---Init Complete---\n\r");

  while (1) {
    io_joystick_read_position(&joy, &pos);
    printf("Joystick Position\tX=%d\tY=%d\n\r", pos.x, pos.y);
  }
  return 0;
}
