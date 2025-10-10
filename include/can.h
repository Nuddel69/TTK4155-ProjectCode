#ifndef INCLUDE_CAN_H_
#define INCLUDE_CAN_H_

#include <stdint.h>
#include <avr/io.h>


#define MCP_CANSTAT
#define MODE_MASK    
#define CAN_CS       //Dont know whichone we are using yet
#define MCP_READ     0b00000011
#define CAN_STCONFIG 0b00000110

struct can_msg{
    uint8_t adress;
    uint8_t num_df;
    uint8_t data;
};

//See page 59 datasheet on controller
enum MODE{
    MODE_NORMAL      = 0b000,
    MODE_SLEEP       = 0b001,
    MODE_LOOPBACK    = 0b010,
    MODE_LISTEN_ONLY = 0b011,
    MODE_CONFIG      = 0b100
}

enum CAN_IRQ{
    000 = NO_IRQ    = 0b000,
    001 = ERROR_IRQ = 0b001,
    010 = WAKEUP_IRQ = 0b010,
    011 = TXB0_IRQ  = 0b011,
    100 = TXB1_IRQ  = 0b100,
    101 = TXB2_IRQ  = 0b101,
    110 = RXB0_IRQ  = 0b110,
    111 = RXB1_IRQ  = 0b111
}

enum CAN_BUF_ADR{
    TXB0SIDH = 0x31,
    TXB0D0   = 0x36,
    TXB1SIDH = 0x41,
    TXB1D0   = 0x46,
    TXB2SIDH = 0x51,
    TXB2D0   = 0x56,
    RXB0SIDH = 0x61,
    RXB0D0   = 0x66,
    RXB1SIDH = 0x71,
    RXB1D0   = 0x76,
}


//------------------//
//   GENERAL CAN    //
//------------------//

int8_t CAN_init(void);
int8_t CAN_read(void);
int8_t CAN_reset(void);


//------------------//
//   MSCP2515    //
//------------------//

int8_t init_MSCP2515(struct io_oled_device *dev);
int8_t MSCP2515_read(struct *dev,uint8_t rx_buf_num, uint8_t *out);
int8_t MSCP2515_write(*dev,uint8_t *data);
int8_t MSCP2515_request_to_send(struct *dev,uint8_t tx_buf_num);
int8_t MSCP2515_bit_modify(struct *dev, uint8_t reg, uint8_t set_val);
int8_t MSCP2515_reset(struct io_oled_device *dev);
int8_t MSCP2515_read_status(struct io_oled_device *dev);

#endif // INCLUDE_CAN_H_