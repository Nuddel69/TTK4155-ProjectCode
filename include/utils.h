#ifndef INCLUDE_INCLUDE_UTILS_H_
#define INCLUDE_INCLUDE_UTILS_H_

#include <avr/io.h>

#define UTILS_DEBUG_LED_PIN PB0

#define NULL ((void *)0)

#define STATUS_ASSERT(status)                                                  \
  {                                                                            \
    DDRB |= (1 << UTILS_DEBUG_LED_PIN);                                        \
                                                                               \
    if (status) {                                                              \
      PORTB |= (1 << UTILS_DEBUG_LED_PIN);                                     \
      return -1;                                                               \
    }                                                                          \
  }

int utils_test_address_decode(void);
int utils_assert(int status);
/* int8_t map(uint8_t x, uint8_t in_min, uint8_t in_max, int8_t out_min,
           int8_t out_max); */

int16_t map(uint8_t x, uint8_t in_min, uint8_t in_max, int16_t out_min,
           int16_t out_max);

#endif // INCLUDE_INCLUDE_UTILS_H_
