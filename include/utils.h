#ifndef INCLUDE_INCLUDE_UTILS_H_
#define INCLUDE_INCLUDE_UTILS_H_

#include <avr/io.h>
#include <util/delay.h>

#include "io.h"

#define UTILS_DEBUG_LED_PIN PB0

#define TRASHCAN ((void *)0)

#define STATUS_ASSERT(status)                                                  \
  {                                                                            \
    DDRB |= (1 << UTILS_DEBUG_LED_PIN);                                        \
                                                                               \
    if (status) {                                                              \
      PORTB |= (1 << UTILS_DEBUG_LED_PIN);                                     \
    }                                                                          \
    while (status > 0) {                                                       \
      PORTB ^= (1 << UTILS_DEBUG_LED_PIN);                                     \
      _delay_ms(500);                                                          \
    }                                                                          \
  }

uint8_t utils_joystick_print_pos(struct io_joystick_position *pos);
uint8_t utils_joystick_print_raw(struct io_joystick_device *dev);

int16_t map(uint8_t x, uint8_t in_min, uint8_t in_max, int16_t out_min,
            int16_t out_max);

#endif // INCLUDE_INCLUDE_UTILS_H_
