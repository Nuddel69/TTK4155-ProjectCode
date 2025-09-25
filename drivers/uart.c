#include <avr/interrupt.h>
#include <avr/io.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>

#include "uart.h"

char receive_buf = 0;
ISR(USART0_RXC_vect) { USART_Receive(); }

int USART_init(struct USART_config *config) {
  uint16_t ubrr = (config->fosc / 16 / config->baud) - 1;
  UBRR0H = (unsigned char)(ubrr >> 8);
  UBRR0L = (unsigned char)ubrr;

  // Enable RX & TX & RXC interrupt
  UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);

  // Format 9600 8N1
  UCSR0C = (1 << URSEL0) | (1 << USBS0) | (3 << UCSZ00);

  sei();
  fdevopen(USART_Transmit, USART_Receive);

  return 0;
}

int USART_Transmit(unsigned char data) {
  // Wait for empty transmit buffer
  while (!(UCSR0A & (1 << UDRE0)))
    ;

  // Put data into buffer, sends the data
  UDR0 = data;

  return 0;
}

// This function is a big memory nono!
int USART_SendString(char *data) {
  int status;
  int counter = 0;
  while (data[counter] != '\0') {
    USART_Transmit(data[counter]);
    counter++;
  }
  status = USART_Transmit('\x0D');
  status = USART_Transmit('\x0A');

  return status;
}

int USART_Receive(void) {
  // Get and return received data from buffer
  receive_buf = UDR0;

  return 0;
}

int USART_ReceiveHandler() {
  char cmd[32] = {0};
  int cmd_count = 0;

  // Echo
  if (receive_buf) {
    USART_Transmit(receive_buf);
    if (receive_buf == 0x0D) {
      USART_Transmit(0x0A);
    }

    cmd[cmd_count] = receive_buf;
    cmd_count++;
    receive_buf = 0;
  }

  // CMD Handler
  if (cmd[cmd_count] == 0x0D) {
    USART_SendString(cmd);
    cmd[cmd_count] = 0;
    cmd_count = 0;
  }

  return 0;
}
