#include "avr/common.h"
#include "avr/io.h"
#include "uart.h"
#include <stddef.h>
#include <util/delay.h>

#define FOSC 4915200
#define BAUD 9600

struct USART_config config = {BAUD, FOSC};

int main() {
  USART_init(&config);

  while (1) {
    USART_SendString("Hello World!");
    _delay_ms(100);
  }
  return 0;
}
