#include <stddef.h>
#include <util/delay.h>

#include "uart.h"
#include "utils.h"
#include "xmem.h"

#define FOSC 4915200
#define BAUD 9600

struct USART_config config = {BAUD, FOSC};

int main() {
  int status = 0;

  status = USART_init(&config);
  STATUS_ASSERT(status)

  status = xmem_init();
  STATUS_ASSERT(status)

  status = SRAM_test();
  STATUS_ASSERT(status)

  while (1) {
    // USART_ReceiveHandler(); // Required for USART-echo and command handling
    USART_SendString("Testing decode: ADC -> SRAM");
    status = utils_test_address_decode();
    STATUS_ASSERT(status)
    USART_SendString("Finished decode test");
  }
  return 0;
}
