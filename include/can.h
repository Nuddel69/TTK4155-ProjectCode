/**
 * @file can.h
 * @brief CAN driver
 * @defgroup CAN CAN
 * @ingroup Communication
 * @{
 */

#ifndef INCLUDE_CAN_H_
#define INCLUDE_CAN_H_

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

#include "spi.h"

#define CAN_ID_ERROR 0x01
#define CAN_ID_GAMEOVER 0x02
#define CAN_ID_GAMESTART 0x03
#define CAN_ID_JOYPOS 0x04
#define CAN_ID_RESET 0x05
#define CAN_ID_READY 0x06
#define CAN_ID_SCORE 0x07
#define CAN_ID_DEFAULT 0x08

#define CAN_RXQ_SIZE 8

/**
 * @brief Represents a simple CAN message with an address, data count, and a
 * single byte of data.
 */
struct can_msg {
  uint8_t adress; /**< Node address or identifier of the message */
  uint8_t num_df; /**< Number of data frames (or message fragments) */
  uint8_t data;   /**< Single data byte payload */
};

/**
 * @brief Represents a CAN device, storing the SPI slave controlling the
 * MCP2515.
 */
struct can_device {
  enum spi_slave spi; /**< SPI slave select line for this device */
};

/**
 * @brief Represents a full CAN frame for sending/receiving.
 *
 * Supports standard 11-bit or extended 29-bit IDs, data length code,
 * payload, and Remote Transmission Request (RTR) flag.
 */
struct CAN_frame {
  uint32_t id;      /**< 11-bit or 29-bit identifier (extended = true) */
  uint8_t dlc;      /**< Data length code (0-8 bytes) */
  char data[8];     /**< Payload bytes (up to 8) */
  uint8_t extended; /**< Flag for extended (29-bit) ID; false = 11-bit */
  uint8_t rtr; /**< Remote Transmission Request flag; true asks other node for a
                  response */
};

//------------------//
//   GENERAL CAN    //
//------------------//

/**
 * @brief Initialize a CAN device.
 *
 * Configures MCP2515 and sets up AVR external interrupt for CAN receive events.
 *
 * @param dev Pointer to the CAN device structure.
 * @return 0 on success, negative on failure.
 */
int8_t can_init(struct can_device *dev);

/**
 * @brief Write a CAN frame to the MCP2515 transmit buffer.
 *
 * Handles standard and extended IDs and requests the controller to send the
 * message.
 *
 * @param dev Pointer to the CAN device structure.
 * @param msg The CAN frame to send.
 * @return 0 on success, negative on error.
 */
int8_t can_write(struct can_device *dev, struct CAN_frame msg);

/**
 * @brief Read a message from RX buffer 0.
 *
 * Extracts identifier, data length, and payload from the MCP2515 RXB0 buffer.
 *
 * @param dev Pointer to the CAN device.
 * @param out Pointer to CAN_frame to store the received message.
 * @return 0 on success.
 */
int8_t can_read_rx0(struct can_device *dev, struct CAN_frame *out);

/**
 * @brief Read a message from RX buffer 1.
 *
 * Extracts identifier, data length, and payload from the MCP2515 RXB1 buffer.
 *
 * @param dev Pointer to the CAN device.
 * @param out Pointer to CAN_frame to store the received message.
 * @return 0 on success.
 */
int8_t can_read_rx1(struct can_device *dev, struct CAN_frame *out);

/**
 * @brief Add a CAN frame to the internal receive queue.
 *
 * Used by interrupt routines to buffer incoming messages.
 *
 * @param msg Pointer to the CAN_frame to enqueue.
 * @return 1 on success, 0 if queue is full.
 */
int can_rxq_add(struct CAN_frame *msg);

/**
 * @brief Pull a CAN frame from the internal receive queue.
 *
 * @param out Pointer to the CAN_frame to fill with dequeued data.
 * @return 1 on success, 0 if queue is empty.
 */
int can_rxq_pull(struct CAN_frame *out);

int8_t can_reset(struct can_device *dev, uint8_t address,
                 struct CAN_frame data_frame);

//------------------//
//   MSCP2515    //
//------------------//

int8_t MCP2515_init(struct can_device *dev);
int8_t MCP2515_read(struct can_device *dev, uint8_t rx_buf_num, uint8_t *out);
int8_t MCP2515_write(struct can_device *dev, uint8_t addr, uint8_t data);
int8_t MCP2515_request_to_send(struct can_device *dev, uint8_t tx_buf_num);
int8_t MCP2515_bit_modify(struct can_device *dev, uint8_t reg, uint8_t mask,
                          uint8_t set_val);
int8_t MCP2515_reset(struct can_device *dev);
int8_t MCP2515_read_status(struct can_device *dev, uint8_t *out);

#endif // INCLUDE_CAN_H_

/** @} */
