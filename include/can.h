#ifndef INCLUDE_CAN_H_
#define INCLUDE_CAN_H_

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

#include "spi.h"

#define CAN_ID_ERROR     0x01
#define CAN_ID_GAMEOVER  0x02
#define CAN_ID_GAMESTART 0x03
#define CAN_ID_JOYPOS    0x04
#define CAN_ID_SOLONOID  0x05
#define CAN_ID_MOTORPOS  0x06
#define CAN_ID_SCORE     0x07
#define CAN_ID_DEFAULT   0x08

#define CAN_RXQ_SIZE 16

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

//------------------//
//   GENERAL CAN    //
//------------------//

int8_t can_init(struct can_device *dev);
int8_t can_read(struct can_device *dev, struct CAN_frame *out);
int8_t can_write(struct can_device *dev, struct CAN_frame msg);
int8_t can_reset(struct can_device *dev, uint8_t address,
                 struct CAN_frame data_frame);

/*				 
int8_t tx_joy_btn(struct io_joystick_device *joy_dev,struct io_avr_device *avr_dev, struct can_device *can_dev);
int8_t tx_gamestart(struct can_device *can_dev);
int8_t tx_error(struct can_device *can_dev);

int8_t process_can_frame(struct CAN_frame *can_frame);
*/
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

static inline int can_rxq_add(struct CAN_frame *msg);
int can_rxq_pull(struct CAN_frame *out);

#endif // INCLUDE_CAN_H_
