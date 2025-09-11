#include "uart.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

void USART_init(struct USART_config *config) {

  uint16_t ubrr = (config->fosc / 16 / config->baud) - 1;
  UBRR0H = (unsigned char)(ubrr >> 8);
  UBRR0L = (unsigned char)ubrr;

  // Enable RX & TX & RXC interrupt
  UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);

  // Format 9600 8N1
  UCSR0C = (1 << URSEL0) | (1 << USBS0) | (3 << UCSZ00);

  sei();
}

void USART_Transmit(unsigned char data) {
  // Wait for empty transmit buffer
  while (!(UCSR0A & (1 << UDRE0)))
    ;

  // Put data into buffer, sends the data
  UDR0 = data;
}

// This function is a big memory nono!
void USART_SendString(char *data) {
  int counter = 0;
  while (data[counter] != '\0') {
    USART_Transmit(data[counter]);
    counter++;
  }
  USART_Transmit('\x0D');
  USART_Transmit('\x0A');
}

// uint8_t USART_Receive(uint8_t *buf) {
//   // Get and return received data from buffer
//   return UDR0;
// }
