#include "uart.h"
#include "xmem.h"
#include <stddef.h>
#include <util/delay.h>
#include <avr/io.h>

#define FOSC 4915200
#define BAUD 9600

struct USART_config config = {BAUD, FOSC};

int main() {

  USART_init(&config);
  xmem_init();
  SRAM_test();

  DDRB |= (1 << PB0);   // configure PB0 as output

  while (1) {
    //USART_ReceiveHandler(); // Required for USART-echo and command handling
    USART_SendString("world hello");
    _delay_ms(1000);
    PORTB ^= (1 << PB0);
  }
  return 0;
}
