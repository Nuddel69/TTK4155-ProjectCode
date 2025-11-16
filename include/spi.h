/**
 * @file spi.h
 * @brief SPI driver
 * @defgroup SPI SPI
 * @ingroup Hardware
 * @{
 */

#ifndef INCLUDE_INCLUDE_SPI_H_
#define INCLUDE_INCLUDE_SPI_H_

/**
 * @brief Identifiers for available SPI slave devices.
 */
enum spi_slave {
  SSB2, /**< Slave on PB2 */
  SSB3, /**< Slave on PB3 */
  SSE2  /**< Slave on PE2 */
};

/**
 * @brief Initialize SPI in master mode.
 *
 * Configures MOSI, SCK, and SS pins, sets clock prescaler, and ensures
 * all slave-select lines default to inactive.
 *
 * @return 0 on success, non-zero on failure.
 */
int spi_init(void);

/**
 * @brief Check if the SPI subsystem is initialized.
 *
 * @return 0 if ready, non-zero otherwise.
 */
int spi_ready(void);

/**
 * @brief Send a sequence of bytes to a slave without reading response.
 *
 * @param[in] slave Pointer to the target slave device.
 * @param[in] data Buffer of bytes to transmit.
 * @param[in] length Number of bytes to send.
 * @return 0 on success.
 */
int spi_send_n(enum spi_slave *slave, unsigned char *data, int length);

/**
 * @brief Send a single byte to a slave.
 *
 * @param[in] slave Pointer to target slave.
 * @param[in] data Byte to transmit.
 * @return 0 on success.
 */
int spi_send(enum spi_slave *slave, unsigned char data);

/**
 * @brief Receive one byte from a slave.
 *
 * Transmits dummy data and stores the received byte.
 *
 * @param[in]  slave Pointer to target slave.
 * @param[out] out   Pointer to received byte.
 * @return 0 on success.
 */
int spi_recieve(enum spi_slave *slave, unsigned char *out);

/**
 * @brief Receive several bytes from a slave.
 *
 * @param[in]  slave Pointer to target slave.
 * @param[out] out   Buffer to store received data.
 * @param[in]  length Number of bytes to read.
 * @return 0 on success.
 */
int spi_recieve_n(enum spi_slave *slave, unsigned char *out, int length);

/**
 * @brief Full-duplex SPI transfer.
 *
 * Sends `length` bytes and reads a response simultaneously.
 *
 * @param[in]  slave Pointer to slave device.
 * @param[in]  data  Transmit buffer, or TRASHCAN for dummy writes.
 * @param[out] out   Receive buffer, or TRASHCAN to discard input.
 * @param[in]  length Number of bytes to exchange.
 * @return 0 on success.
 */
int spi_duplex(enum spi_slave *slave, unsigned char *data, unsigned char *out,
               int length);

/**
 * @brief Set the slave-select line for the given device.
 *
 * @param[in] slave Pointer to slave identifier.
 * @param[in] state 0 = select (active), 1 = deselect.
 * @return 0 on success, -1 if slave is invalid.
 */
int spi_set_slave_select(enum spi_slave *slave, unsigned char state);

/**
 * @brief Push one byte to a slave and optionally read the response.
 *
 * @param[in]  slave Pointer to target slave.
 * @param[in]  data  Byte to send.
 * @param[out] out   Pointer to received byte (or TRASHCAN to ignore).
 * @return 0 on success.
 */
int spi_push(enum spi_slave *slave, unsigned char data, unsigned char *out);

#endif // INCLUDE_INCLUDE_SPI_H_

/** @} */
