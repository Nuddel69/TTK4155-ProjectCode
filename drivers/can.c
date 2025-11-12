#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#include "can.h"
#include "log.h"
#include "mcp.h"
#include "spi.h"

LOG_MODULE_DEFINE("CAN")

struct can_device *can_irq;
struct CAN_frame new_message;

static volatile uint8_t rxq_head = 0, rxq_tail = 0;
struct CAN_frame rxq[CAN_RXQ_SIZE];

//------------------//
//   GENERAL CAN    //
//------------------//
int8_t can_init(struct can_device *dev) {

  can_irq = dev;
  struct CAN_frame new_message = {0x00, 0x01, '1', 0, 0};

  // Set PE0 as input
  DDRE &= ~(1 << PE0);
  PORTE |= (1 << PE0);

  // Disable global interrupts
  cli();
  // Enable INT2
  GICR |= (1 << CAN_INTERRUPT_ISR_REGISTER);

  // Edge select, active-low interrupt from MCP2515
  EMCUCR &= ~(1 << ISC2);

  // Configure interrupt falling edge
  // MCUCR |=(1<<ISC01);
  // MCUCR &= ~(1<<ISC00);
  // Enable global interrupts
  sei();

  return MCP2515_init(dev);
}

int8_t can_write(struct can_device *dev, struct CAN_frame msg) {
	
	
if (msg.extended) {
  // 29bit ID 
  uint32_t id = msg.id & 0x1FFFFFFF;

  uint8_t EID0 =  id        & 0xFF;         // EID[7:0]
  uint8_t EID8 = (id >> 8)  & 0xFF;         // EID[15:8]
  uint8_t SIDL = ((id >> 16) & 0x03)        // EID[17:16] -> SIDL<1:0>
  | 0x08                                    // EXIDE=1 (bit3)
  | ((id >> 18) & 0xE0);                    // SID[2:0]  -> SIDL<7:5>
  uint8_t SIDH = (id >> 21) & 0xFF;         // SID[10:3] -> SIDH

  MCP2515_write(dev, MCP2515_TXB0SIDH, SIDH);
  MCP2515_write(dev, MCP2515_TXB0SIDL, SIDL);
  MCP2515_write(dev, MCP2515_TXB0EID8, EID8);
  MCP2515_write(dev, MCP2515_TXB0EID0, EID0);
} else { // 11bit ID
  uint8_t ID_MSB = (0x7F8 & msg.id) >> 3;
  uint8_t ID_LSB = (0x7 & msg.id) << 5;

  MCP2515_write(dev, MCP2515_TXB0SIDH, ID_MSB);
  MCP2515_write(dev, MCP2515_TXB0SIDL, ID_LSB);
}
  MCP2515_write(dev, TXB0DLC, msg.dlc);

  uint8_t buff0_status;
  MCP2515_read(dev, MCP2515_TXB0CTRL, &buff0_status);
  if ((buff0_status & 0x8) != 0x8) {
    if (msg.dlc > 8) {
      LOG_ERR("TX Buffer0 overflow, message too large", -2)
      return -2;
    }
    for (uint8_t i = 0; i < msg.dlc; i++) {
      MCP2515_write(dev, MCP2515_TXB0D0 + i, msg.data[i]);
    }

    MCP2515_request_to_send(dev, MCP2515_RTS_TX0);
    return 0;

  } else {
    // printf("TX Buffer0 not avaliable\r\n");
    return -2;
  }
}

int8_t can_read(struct can_device *dev, struct CAN_frame *out) {

  uint8_t ID_MSB;
  uint8_t ID_LSB;
  uint8_t length;

  MCP2515_read(dev, MCP2515_RXB0SIDH, &ID_MSB);
  MCP2515_read(dev, MCP2515_RXB0SIDL, &ID_LSB);

  ID_LSB = (ID_LSB & 0xE0) >> 5;
  out->id = ID_MSB << 3;
  out->id = (out->id & 0x7F8) | (ID_LSB & 0x7);

  MCP2515_read(dev, MCP2515_RXB0DLC, &length);
  out->dlc = (length & 0xF);
  if (out->dlc > 8) {
    out->dlc = 8;
  }

  for (uint8_t i = 0; i < out->dlc; i++) {
    MCP2515_read(dev, MCP2515_RXB0D0 + i, (uint8_t *)&out->data[i]);
  }
  // Clear RX0IF to clear buffer for next message
  MCP2515_bit_modify(dev, MCP2515_CANINTF, (1 << 0), 0x00); // RX0IF=bit0
  return 0;
}


static inline int can_rxq_add(struct CAN_frame *msg) {
	
	uint8_t next = (uint8_t)((rxq_head + 1) & (CAN_RXQ_SIZE - 1));
	if (next == rxq_tail){
		return 0;
	}
	rxq[rxq_head] = *msg;
	rxq_head = next;
	return 1;
}

// Pull from RX buffer
int can_rxq_pull(struct CAN_frame *out){
	if (rxq_tail == rxq_head){
		return 0;
	}
	*out = rxq[rxq_tail];
	rxq_tail = (uint8_t)((rxq_tail + 1) & (CAN_RXQ_SIZE - 1));
	return 1;
}


int tx_joy_btn(struct io_joystick_device *joy_dev,struct io_avr_device *avr_dev, struct can_device *can_dev){
	
	//Read ADC
	struct io_joystick_position joy_pos;
	io_joystick_read_position(joy_dev, &joy_pos);
	
	struct io_avr_buttons btn;
	io_avr_buttons_read(avr_dev,&btn);
	
	//Frame Data
	struct CAN_frame msg = {CAN_ID_JOYPOS,0x08,{joy_pos.x,joy_pos.y,btn.right,btn.left,btn.nav,0,0,0},0, 0};
	
	//Transmit data
	can_write(&can, msg);
	
	
	return 0;
}

int tx_gamestart(struct can_device *can_dev){
	
	//Frame Data
	struct CAN_frame msg = {CAN_ID_GAMESTART,0x08,{0,0,0,0,0,0},1, 0};
	
	//Transmit data
	can_write(&can, msg);
	return 0;
}

int tx_error(struct can_device *can_dev){
	
	//Frame Data
	struct CAN_frame msg = {CAN_ID_ERROR,0x08,{0,0,0,0,0,0},1, 0};
	
	//Transmit data
	can_write(&can, msg);
	return 0;
}

int process_can_frame(struct CAN_frame *can_frame){
	
	switch (can_frame->id){
		
		case CAN_ID_ERROR:{   //This ID is reserved for errors, BOTH node1 and node2
			
			STATUS_ASSERT(1);
			break;
		}
		case CAN_ID_GAMEOVER:{//This ID is reserved for gameover message from node2
			
			break;
		}
		case CAN_ID_GAMESTART:{//This ID is reserved for starting a new game from node1
			
			break;
		}
		case CAN_ID_JOYPOS:{  //This ID is reserved for sending Joystick position and button state
			
			break;
		}
		case CAN_ID_SOLONOID:{//This ID is reserved for sending trigger signal for the solonoid
			
			break;
		}
		case CAN_ID_MOTORPOS:{//This ID is reserved for sending current motor position
			
			break;
		}
		case CAN_ID_SCORE:{   //This ID is reserved for sending gamescore
			
			break;
		}
		case CAN_ID_DEFAULT:{ //This ID is for anything else
			break;
		}
		default:{
			
		}
	}
	
}

//------------------//
// MCP2515 Specific //
//------------------//

// External interrupt PE0, From MCP2515
ISR(INT2_vect) {

  LOG_INF("External interrupt")
  uint8_t status;
  MCP2515_read_status(can_irq, &status);

  if (status & rx_buff_0_full) {
    can_read(can_irq, &new_message);
	can_rxq_add(&new_message);
    LOG_INF("RX0 Full");
  }

  if (status & rx_buff_1_full) {
    can_read(can_irq, &new_message);
	can_rxq_add(&new_message);
    LOG_INF("RX1 Full");
  }

  if (status & tx_buff_0_busy) {
    LOG_INF("TX0 Busy");
  }

  if (status & tx_buff_0_empty) {
    LOG_INF("TX0 Empty");
  }

  // reset CANINTF
  MCP2515_write(can_irq, MCP2515_CANINTF, 0x00);
}

int8_t MCP2515_init(struct can_device *dev) {

  while (spi_ready()) { // Confirm SPI Initialized
    uint8_t status = spi_init();
  }

  MCP2515_reset(dev); // Send reset - command
  uint8_t value = 0;
  _delay_ms(10);

  // Confirm that we are in correct mode after boot, and that there are no
  // pending messages.
  MCP2515_read(dev, 0x0E, &value);

  // Ensure config mode after reset
  if ((value & MODE_MASK) != (MODE_CONFIG)) {
    LOG_ERR("SPI to CAN controller is not in configuration mode after reset!",
            -1);
    return -1;
  }

  // MCP2515_read_status(dev, &value);
  if ((value & 0x0E) != (MCP2515_NO_IRQ)) {
    LOG_ERR("There is an interrupt request when booting the SPI-CAN controller",
            -2);
    return -2;
  }

  // RTS control
  unsigned char tx[3] = {MCP2515_WRITE, MCP2515_TXRTSCTRL, MCP2515_TXRTS_CONF};
  spi_send_n(&dev->spi, tx, 3);

  // Receive buffer0 config. Receive all, no overflow
  MCP2515_bit_modify(dev, MCP2515_RXB0CTRL, 0x60, 0x60);
  MCP2515_read(dev, MCP2515_RXB0CTRL, &value);
  if (value != 0x60) {
    LOG_ERR("Couldnt configure MCP2515 RX0 Buffer", -3);
    return -3;
  }

  // Receive buffer1 config. Receive all
  MCP2515_bit_modify(dev, MCP2515_RXB1CTRL, 0x60, 0x60);
  MCP2515_read(dev, MCP2515_RXB1CTRL, &value);
  if (value != 0x60) {
    LOG_ERR("Couldnt configure MCP2515 RX1", -4);
    return -4;
  }

  // Set PE0 as input
  DDRE &= ~(1 << PE0);
  PORTE |= (1 << PE0);

  // Enable INT2
  GICR |= (1 << INT2);

  // Edge select, active-low interrupt from MCP2515
  EMCUCR &= ~(1 << ISC2);

  // Interrupt config: msg error, error flag change, TX0 empty, RX0 full
  MCP2515_bit_modify(dev, MCP2515_CANINTE, 0xFF, 0x5);
  MCP2515_read(dev, MCP2515_CANINTE, &value);
  if (value != 0x5) {
    LOG_ERR("Couldnt set IRQ config for MCP2515", -5);
    return -5;
  }

  // BRP and CAN timing config
  MCP2515_bit_modify(dev, MCP2515_CNF1, MCP2515_VAL_CNF1, MCP2515_VAL_CNF1);
  MCP2515_bit_modify(dev, MCP2515_CNF2, MCP2515_VAL_CNF2, MCP2515_VAL_CNF2);
  MCP2515_bit_modify(dev, MCP2515_CNF3, MCP2515_VAL_CNF3, MCP2515_VAL_CNF3);

  MCP2515_read(dev, MCP2515_CNF1, &value);
  if (value != MCP2515_VAL_CNF1) {
    LOG_ERR("Couldnt configure BRP for MCP2515", -6);
    return -6;
  }

  MCP2515_read(dev, MCP2515_CNF2, &value);
  if (value != MCP2515_VAL_CNF2) {
    LOG_ERR("CNF2 not set", -7);
    return -7;
  }

  MCP2515_read(dev, MCP2515_CNF3, &value);
  if (value != MCP2515_VAL_CNF3) {
    LOG_ERR("CNF3 not set", -8);
    return -8;
  }

  // Set the controller to normal mode !!!THIS HAS TO BE LAST IN INIT!!!
  MCP2515_write(dev, MCP2515_CANCTRL, MODE_NORMAL);
  MCP2515_read(dev, MCP2515_CANSTAT, &value);
  if ((value & MODE_MASK) != MODE_NORMAL) {
    LOG_ERR("MCP2515 could not be set to NORMAL mode when init completed", -9);
    return -9;
  }

  return 0;
}

// Write data to register beginning at selected address.
int8_t MCP2515_write(struct can_device *dev, uint8_t addr, uint8_t data) {

  unsigned char frame[3] = {MCP2515_WRITE, addr, data};
  spi_send_n(&dev->spi, frame, 3);

  return 0;
}

int8_t MCP2515_write_n(struct can_device *dev, uint8_t addr, uint8_t *data) {

  unsigned char frame[10] = {MCP2515_WRITE, addr,    data[0], data[1], data[2],
                             data[3],       data[4], data[5], data[6], data[7]};
  spi_send_n(&dev->spi, frame, 2 + 8);

  return 0;
}

// Read a specefic RX BUFFER see operation on pg 66 in CAN controller datasheet
int8_t MCP2515_read(struct can_device *dev, uint8_t addr, uint8_t *out) {

  // Make sure we are writing to somthing
  if (!out) {
    return -1;
  }

  unsigned char tx[3] = {MCP2515_READ, addr, 0xff};
  unsigned char rx[3];
  // spi_duplex(&dev->spi, tx, rx, 3);
  spi_push(&dev->spi, tx[0], NULL);
  spi_push(&dev->spi, tx[1], NULL);
  spi_recieve(&dev->spi, out);

  return 0;
}

// Instructs controller to begin message transmission sequence for
// any of the transmit buffers.
int8_t MCP2515_request_to_send(struct can_device *dev, uint8_t buffer) {

  spi_send(&dev->spi, buffer);

  return 0;
}

// Set or clear individual bits in a particular register
int8_t MCP2515_bit_modify(struct can_device *dev, uint8_t reg, uint8_t mask,
                          uint8_t set_val) {

  unsigned char cmd[4] = {MCP2515_BITMOD, reg, mask, set_val};
  spi_duplex(&dev->spi, cmd, NULL, 4);
  // spi_send_n(&dev->spi, cmd, 4);

  return 0;
}

// Resets internal registers to default state, this sets Config mode
int8_t MCP2515_reset(struct can_device *dev) {

  spi_send(&dev->spi, MCP2515_RESET);

  return 0;
}

// Quick polling command that reads several status bits for transmit and receive
// functions.
int8_t MCP2515_read_status(struct can_device *dev, uint8_t *out) {

  uint8_t *status;

  spi_push(&dev->spi, MCP2515_READ_STATUS, NULL);
  spi_recieve(&dev->spi, out);

  return 0;
}
