#include "uart.h"
#include <stddef.h>
#include <util/delay.h>

#define FOSC 4915200
#define BAUD 9600

struct USART_config config = {BAUD, FOSC};

int main() {

  USART_init(&config);

  while (1) {
    USART_ReceiveHandler(); // Required for USART-echo and command handling
  }
  return 0;
}
