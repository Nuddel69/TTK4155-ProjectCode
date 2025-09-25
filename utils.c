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
