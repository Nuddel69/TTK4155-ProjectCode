#include "avr/io.h"
#include <util/delay.h>

int main() {
  //DDRB |= (1 << DDB5);
  USART_init(9600);
  while (1) {
    /*
    PORTB |= (1 << PB5);
    _delay_ms(500);
    PORTB &= ~(1 << PB5);
    _delay_ms(500);
    */
    USART_Transmit('k');

  }
  return 0;
}

void USART_init(BAUDRATE){

// Set baudrate, 31=9600,
uint_8 UBBR_Val = (4.9152*10^6)/(16*BAUDRATE)-1;

//UBRRH =  UBBR_Val >> 8;
UBRRL =  UBBR_VAL;

UBBRH = UBBR_Val << 8;

//Enable RX & TX
UCSRB = (1<<RXEN)|(1<<TXEN);

//Format 9600 8N1
UCSRC = (1<<URSEL)|(1<<UCSZ0)|(1<<USCZ1);
}

void USART_Transmit(unsigned char data)
{

// Wait for empty transmit buffer
while (!(UCSRA & (1<<UDRE)));

// Put data into buffer, sends the data
UDR = data;
}


uint8_t USART_Receive(void)
{

// Wait for data to be received
while (!(UCSRA & (1<<RXC)));

// Get and return received data from buffer
return UDR;
}