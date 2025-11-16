/**
 * @file uart.h
 * @brief UART communication driver
 * @todo  Use SRAM to buffer UART transmission
 * @defgroup UART UART
 * @ingroup Communication
 * @{
 */

#ifndef INCLUDE_INCLUDE_UART_H_
#define INCLUDE_INCLUDE_UART_H_

#include <stdint.h>

/**
 * @brief USART configuration structure.
 *
 * @param baud Desired baud rate for UART communication.
 * @param fosc CPU clock frequency (Hz).
 * @param recieve_buffer Temporary buffer for received characters.
 */
struct USART_config {
  int16_t baud;
  long fosc;
  char recieve_buffer;
};

/**
 * @brief Initialize USART0 with the given configuration.
 *
 * Enables TX and RX with frame format 8N1. TODO: Verify...
 *
 * @param[in] config Pointer to configuration containing baud rate and clock.
 * @return 0 on success, non-zero on error.
 */
int USART_init(struct USART_config *config);

/**
 * @brief Transmit CRLF as end-of-line.
 *
 * @return Status code from last character transmitted.
 */
int USART_endl(void);

/**
 * @brief Transmit a single byte via USART0.
 *
 * Blocks until the transmit buffer is ready.
 *
 * @param[in] data Byte to send.
 * @return 0 on success.
 */
int USART_Transmit(unsigned char data);

/**
 * @brief Send a zero-terminated string over USART0.
 *
 * Newlines (`'\n'`) are converted to CRLF.
 *
 * @param[in] data Pointer to zero-terminated string.
 * @return Status code of the last transmitted character.
 */
int USART_SendString(char data[]);

/**
 * @brief Receive one byte from USART0. (Minimal implementation.)
 *
 * Stores the received byte into a global buffer.
 *
 * @return 0 on success.
 */
int USART_Receive(void);

/**
 * @brief Handle incoming received bytes and simple command echoing.
 *
 * Echoes characters and echos back the full command line when CR is received.
 *
 * @return 0 on success.
 */
int USART_ReceiveHandler(void);

#endif // INCLUDE_INCLUDE_UART_H_

/** @} */
