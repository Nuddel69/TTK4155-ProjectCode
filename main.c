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
    _delay_ms(100);
    USART_Transmit('H');
    USART_Transmit('e');
    USART_Transmit('l');
    USART_Transmit('l');
    USART_Transmit('o');
    USART_Transmit(' ');
    USART_Transmit('W');
    USART_Transmit('o');
    USART_Transmit('r');
    USART_Transmit('l');
    USART_Transmit('d');
    USART_Transmit('!');
    USART_Transmit('\x0D');
    USART_Transmit('\x0A');
  }
  return 0;
}
