/**
 * @file utils.h
 * @brief Utility and helper functions for joystick, LED debugging, and mapping.
 * @defgroup UTILS Utilities
 * @ingroup Helpers
 * @{
 */

#ifndef INCLUDE_INCLUDE_UTILS_H_
#define INCLUDE_INCLUDE_UTILS_H_

#include <avr/io.h>
#include <util/delay.h>

#include "io.h"

#define UTILS_DEBUG_LED_PIN PB0 /**< Debug LED pin for error status */

#define TRASHCAN ((void *)0)

/**
 * @brief Macro to assert a status and flash a debug LED
 * @param status Negative sets LED; positive triggers LED flashing; zero does
 * nothing
 */
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

/**
 * @brief Prints joystick position (x/y) via UART
 * @param pos Pointer to joystick position struct
 * @return 0 on success
 */
uint8_t utils_joystick_print_pos(struct io_joystick_position *pos);

/**
 * @brief Prints raw ADC joystick readings via UART
 * @param dev Pointer to joystick device
 * @return 0 on success
 */
uint8_t utils_joystick_print_raw(struct io_joystick_device *dev);

/**
 * @brief Maps a value from one range to another (taken from
 * https://docs.arduino.cc/language-reference/en/functions/math/map/)
 * @param x Input value
 * @param in_min Input range minimum
 * @param in_max Input range maximum
 * @param out_min Output range minimum
 * @param out_max Output range maximum
 * @return Mapped value in output range
 */
int16_t map(uint8_t x, uint8_t in_min, uint8_t in_max, int16_t out_min,
            int16_t out_max);

#endif // INCLUDE_INCLUDE_UTILS_H_

/** @} */
