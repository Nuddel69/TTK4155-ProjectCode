#include "avr/common.h"
#include "avr/interrupt.h"
#include "avr/io.h"
#include "uart.h"
#include <stddef.h>
#include <stdint.h>
#include <util/delay.h>

#define FOSC 4915200
#define BAUD 9600

struct USART_config config = {BAUD, FOSC};

// TODO: Move to USART driver
void USART_Receive(void);                 // TODO: Move to USART driver
volatile char receive_buf = 0;            // TODO: Move to USART driver
ISR(USART0_RXC_vect) { USART_Receive(); } // TODO: Move to USART driver
void USART_Receive(void) {                // TODO: Move to USART driver
  // Get and return received data from buffer // TODO: Move to USART driver
  receive_buf = UDR0; // TODO: Move to USART driver

  // return 0; // TODO: Move to USART driver
}

int main() {

  USART_init(&config);

  while (1) {

    // TODO: Move to USART driver
    if (receive_buf) {             // TODO: Move to USART driver
      USART_Transmit(receive_buf); // TODO: Move to USART driver
      receive_buf = 0;             // TODO: Move to USART driver
    } // TODO: Move to USART driver
  }
  return 0;
}
