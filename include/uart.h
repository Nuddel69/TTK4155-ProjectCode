#ifndef INCLUDE_INCLUDE_UART_H_
#define INCLUDE_INCLUDE_UART_H_

#include <stdint.h>

struct USART_config {
  int16_t baud;
  long fosc;
};
void USART_init(struct USART_config *config);
void USART_Transmit(unsigned char data);
uint8_t USART_Receive(void);

#endif // INCLUDE_INCLUDE_UART_H_
