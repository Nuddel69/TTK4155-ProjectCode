#include "utils.h"
#include <util/delay.h>

int utils_test_address_decode(void) {

  volatile char *sram_adr = (char *)0x1800;
  volatile char *adc_adr = (char *)0x1500;

  (void)adc_adr[5];
  _delay_ms(5000);
  (void)sram_adr[5];
  _delay_ms(5000);

  return 0;
}

/* int8_t map(uint8_t x, uint8_t in_min, uint8_t in_max, int8_t out_min,
           int8_t out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
} */

int16_t map(uint8_t x, uint8_t in_min, uint8_t in_max,
            int16_t out_min, int16_t out_max) {
    // do all math in 32-bit to avoid overflow
    int32_t num = (int32_t)(x - in_min) * (out_max - out_min);
    int32_t den = (int32_t)(in_max - in_min);
    int32_t result = num / den + out_min;
    return (int16_t)result;
}