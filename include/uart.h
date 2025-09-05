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
};

/*!
 * \brief Initialize the USART0 interface.
 *
 * \param[in] config A data structure containing the baud rate and clock speed.
 */
void USART_init(struct USART_config *config);

/*!
 * \brief Transmit a single byte.
 *
 * \param[in] data The byte to be transmitted.
 */
void USART_Transmit(unsigned char data);

/*!
 * \brief Use the transmit function to send a string, appending a newline.
 *
 * \param[in] data The string to be sent.
 */
void USART_SendString(char data[]);

/*!
 * \brief Recieve a single byte [NOT IMPLEMENTED.
 *
 * \return status code.
 */
uint8_t USART_Receive(void);

#endif // INCLUDE_INCLUDE_UART_H_
