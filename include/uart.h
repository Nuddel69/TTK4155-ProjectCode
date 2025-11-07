#ifndef INCLUDE_INCLUDE_UART_H_
#define INCLUDE_INCLUDE_UART_H_

#include <stdint.h>

/*!
 * \brief A structure for encapsuling the USART information.
 *
 * \param[in] baud The desired baud rate for the connection.
 * \param[in] fosc The frequency of the crystal oscillator.
 */
struct USART_config {
  int16_t baud;
  long fosc;
  char recieve_buffer;
};

/*!
 * \brief Initialize the USART0 interface.
 *
 * \param[in] config A data structure containing the baud rate and clock speed.
 */
int USART_init(struct USART_config *config);

int USART_print(const char *str, ...);

/*!
 * \brief Transmit a single byte.
 *
 * \param[in] data The byte to be transmitted.
 */
int USART_Transmit(unsigned char data);

/*!
 * \brief Use the transmit function to send a string, appending a newline.
 *
 * \param[in] data The string to be sent.
 */
int USART_SendString(char data[]);

/*!
 * \brief Recieve a single byte [NOT IMPLEMENTED.
 *
 * \return status code.
 */
int USART_Receive(void);

int USART_ReceiveHandler();

#endif // INCLUDE_INCLUDE_UART_H_
