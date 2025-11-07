#include <stddef.h>
#include <util/delay.h>

#include "can.h"
#include "io.h"
#include "log.h"
#include "spi.h"
#include "timer.h"
#include "uart.h"
#include "utils.h"
#include "xmem.h"

#define BAUD 9600

LOG_MODULE_DEFINE("main")

// Device Configs
struct USART_config config = {BAUD, F_CPU};
struct io_joystick_device joy = {0, 1, 0, 0};
struct io_avr_device avr = {SSB3};
struct can_device can = {SSE2};

// Data Containers
struct io_joystick_position pos;
struct CAN_frame can_msg = {0x100, 0x08, "CAN Dumb", 1, 0};
struct io_avr_buttons btn;

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

  status = can_init(&can);
  STATUS_ASSERT(status)

  LOG_INF("---Init complete---")

  while (1) {
    can_write(&can, can_msg);
    // USART_SendString("Hello World!\n");
    _delay_ms(100);
  }
}
