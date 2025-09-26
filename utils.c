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

int8_t map(int8_t x, int8_t in_min, int8_t in_max, int8_t out_min,
           int8_t out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
