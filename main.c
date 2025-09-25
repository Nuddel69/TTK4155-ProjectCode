#include <stddef.h>
#include <util/delay.h>

#include "uart.h"
#include "xmem.h"

#define FOSC 4915200
#define BAUD 9600

struct USART_config config = {BAUD, FOSC};

int main() {
  int status = 0;

  status = USART_init(&config);
  // if (status) {
  //   return -1;
  // }

  status = xmem_init();
  // if (status) {
  //   return -1;
  // }

  status = SRAM_test();
  // if (status) {
  //   return -1;
  // }

  while (1) {
    // USART_ReceiveHandler(); // Required for USART-echo and command handling
  }
  return 0;
}
