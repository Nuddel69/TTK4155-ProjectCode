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

#endif // INCLUDE_INCLUDE_UTILS_H_
