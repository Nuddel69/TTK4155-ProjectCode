#include <stddef.h>
#include <stdio.h>
#include <util/delay.h>

#include "io.h"
#include "spi.h"
#include "timer.h"
#include "uart.h"
#include "utils.h"
#include "xmem.h"

#include "adc.h"
#include "io.h"

#define FOSC 4915200
#define BAUD 9600

struct USART_config config = {BAUD, FOSC};

struct io_joystick_device joy = {0, 1, 0, 0};

struct io_joystick_position pos;

struct io_oled_device oled;

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

  status = io_oled_init(&oled);
  STATUS_ASSERT(status)

  printf("\n\r---Init Complete---\n\r");

  while (1) {
    io_joystick_read_position(&joy, &pos);
    printf("Joystick Position\tX=%d\tY=%d\n\r", pos.x, pos.y);
    _delay_ms(2000);


    struct ADC_meas data;
    ADC_read_all(&data);

    for (int i = 0; i < 4; i++) {
    printf("Channel %d: %d", i, data.channel[i]);
    _delay_ms(2000);
    }
    printf("\n\r");
  }

    /* struct ADC_meas data;
    ADC_read_all(&data);
    struct io_joystick_position *buffer;

    for (int i = 0; i < 4; i++) {
          printf("CH%d raw=%d", i, data.channel[i]);
    }
    

    int raw_x = data.channel[0];
    int raw_y = data.channel[1];

    printf("Raw X=%d Y=%dr", raw_x, raw_y);

    buffer->x = (map(raw_x, 36, 248, -100, 100));
    buffer->y = (map(raw_y, 0, 255, -100, 100));

    printf("Mapped X=%d Y=%d\n\r", buffer->x, buffer->y); */
}
