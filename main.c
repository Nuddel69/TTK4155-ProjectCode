#include "avr/common.h"
#include "avr/io.h"
#include <stddef.h>
#include <util/delay.h>

#define FOSC 4915200
#define BAUD 9600
#define UBRR 31

void USART_init();
void USART_Transmit(unsigned char data);

int main() {
  // DDRB |= (1 << DDB5);
  USART_init();
  while (1) {
    _delay_ms(100);
    USART_Transmit('k');
  }
  return 0;
}

void USART_init() {

  // UBRRH =  UBBR_Val >> 8;
  UBRR0L = 31;

  // Enable RX & TX
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);

  // Format 9600 8N1
  UCSR0C = (1 << URSEL0)| (1<<USBS0)| (1 << UCSZ00) | (1 << UCSZ01);
}

void USART_Transmit(unsigned char data) {

  // Wait for empty transmit buffer
  while (!(UCSR0A & (1 << UDRE0)))
    ;

  // Put data into buffer, sends the data
  UDR0 = data;
}

uint8_t USART_Receive(void) {

  // Wait for data to be received
  while (!(UCSR0A & (1 << RXC0)))
    ;

  // Get and return received data from buffer
  return UDR0;
}
