#ifndef INCLUDE_CAN_H_
#define INCLUDE_CAN_H_

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

#include "spi.h"
/*
#define CAN_CS PE2 // Dont know whichone we are using yet
#define CAN_CS_PORT PORTE
#define CAN_STCONFIG 0b00000110

// Commands for SPI-CAN controller
#define MCP2515_OP_WRITE 0x02
#define MCP2515_OP_READ 0x03
#define MCP2515_OP_BIT_MODIFY 0x05
#define MCP2515_OP_READ_STATUS 0xA0
#define MCP2515_OP_RESET 0xC0
#define MCP2515_OP_RTS_BASE 0x80

#define MCP2515_MODEMASK 0x70
#define MCP2515_IRQMASK 0x0E
*/

struct can_msg {
  uint8_t adress;
  uint8_t num_df;
  uint8_t data;
};

struct can_device {
  enum spi_slave spi;
};

struct CAN_frame {

  uint32_t id; // 11-bit, set extended == true for 29-bit adress
  uint8_t dlc; // 0-8 Data Length Code, How many databytes are in the message
  char data[8];
  uint8_t extended; // Decide ID size, true = 29-bit, false = 11-bit
  uint8_t rtr;      // Remote Transmission Request, must be false for standard
               // message When rtr is true, asks a node with the same ID to
               // respond with a data frame. On the MCP2515, the RTR bit is in
               // the DLC register for TX/RX.
};

/*
// Modes for CAN controller
// See page 59 datasheet on controller
enum MODE {
  MODE_NORMAL = 0b000,
  MODE_SLEEP = 0b001,
  MODE_LOOPBACK = 0b010,
  MODE_LISTEN_ONLY = 0b011,
  MODE_CONFIG = 0b100
};

// CAN interrupt codes
enum CAN_IRQ {
  MCP2515_NO_IRQ = 0,
  MCP2515_ERROR_IRQ = 0b001,
  MCP2515_WAKEUP_IRQ = 0b010,
  MCP2515_TXB0_IRQ = 0b011,
  MCP2515_TXB1_IRQ = 0b100,
  MCP2515_TXB2_IRQ = 0b101,
  MCP2515_RXB0_IRQ = 0b110,
  MCP2515_RXB1_IRQ = 0b111
};


// CAN buffer addresses
enum CAN_BUF_ADR {
  MCP2515_TXB0SIDH = 0x31,
  MCP2515_TXB0D0 = 0x36,
  MCP2515_TXB1SIDH = 0x41,
  MCP2515_TXB1D0 = 0x46,
  MCP2515_TXB2SIDH = 0x51,
  MCP2515_TXB2D0 = 0x56,
  MCP2515_RXB0SIDH = 0x61,
  MCP2515_RXB0D0 = 0x66,
  MCP2515_RXB1SIDH = 0x71,
  MCP2515_RXB1D0 = 0x76,
};

*/

//------------------//
//   GENERAL CAN    //
//------------------//

int8_t can_init(struct can_device *dev);
int8_t can_read(struct can_device *dev, struct CAN_frame *out);
int8_t can_write(struct can_device *dev, struct CAN_frame msg);
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
