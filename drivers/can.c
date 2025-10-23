#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>

#include "can.h"
#include "spi.h"

//------------------//
//   GENERAL CAN    //
//------------------//

int8_t can_init(struct can_device *dev) {
  return MCP2515_init(dev);
}

int8_t can_read(struct can_device *dev, uint8_t address, uint8_t *out) {

  MCP2515_read(dev, address, out);

  return 0;
}

int8_t CAN_write(struct can_device *dev, uint8_t address,
                 struct CAN_frame data_frame) {

  // TXBnSIDH,
  MCP2515_write(dev, 0x31, data_frame.id);
  // TXBnSIDL
  MCP2515_write(dev, 0x32, (data_frame.id << 8));

  uint8_t DLC;
  // TXBnDLC
  if (data_frame.rtr) {
    DLC = (data_frame.dlc | (1 << 6));
  } else {
    DLC = data_frame.dlc;
  }
  MCP2515_write(dev, 0x35, DLC);

  // Set data TXBnDm Data going out from buffer 0
  for (uint8_t i = 0; i < 8; i++) {
    uint8_t adr = 0x36 + i;
    MCP2515_write(dev, adr, data_frame.data[i]);
  }
  
  //Inform controller that we are ready to send on buffer0
  spi_send(dev,0x81);

  return 0;
}
//------------------//
// MCP2515 Specific //
//------------------//

// Interrupt PE0
ISR(INT2_vect) { printf("External interrupt"); }

int8_t MCP2515_init(struct can_device *dev) {
	
  MCP2515_reset(dev); // Send reset - command
  uint8_t value;
  _delay_ms(10);

  // Set bit 6 & 3 in GICR for external interrupt
  // GICR |= 0x24;
  //

  while (spi_ready()) { // Confirm SPI Initialized
    uint8_t status = spi_init();
  }
  

  // Confirm that we are in correct mode after boot, and that there are no
  // pending messages.


  //MCP2515_read(dev,0x0E,&value);
  MCP2515_read_status(dev,&value);
  
  if ((value&0x70)!=(MODE_CONFIG<<5)) {
    printf("SPI to CAN controller is not in configuration mode after reset!\n");
    return 1;
  }

  MCP2515_read_status(dev, &value);
  if ((value&0x0E)!=(MCP2515_NO_IRQ<<1)) {
    printf("There is an interrupt request when booting the SPI-CAN controller");
    return 2;
  }
 

  uint8_t cnf1 = 0x00;
  uint8_t cnf2 = 0xB1;
  uint8_t cnf3 = 0x05;

  // Program bit timing
  MCP2515_write(dev, 0x2A, cnf1);
  MCP2515_write(dev, 0x29, cnf2);
  MCP2515_write(dev, 0x28, cnf3);

  // RX masks: accept all messages. masks = 0, filters = 0
  MCP2515_write(dev, 0x20, 0x00); // RXM0SIDH
  MCP2515_write(dev, 0x21, 0x00); //+1
  MCP2515_write(dev, 0x22, 0x00); //+2
  MCP2515_write(dev, 0x23, 0x00); //+3

  MCP2515_write(dev, 0x24, 0x00); // RXM1SIDH
  MCP2515_write(dev, 0x25, 0x00); //+1
  MCP2515_write(dev, 0x26, 0x00); //+2
  MCP2515_write(dev, 0x27, 0x00); //+3

  // Set filters to zero, RXF0SIDH-RXF5SIDH
  const uint8_t filt_bases[6] = {0x00, 0x04, 0x08, 0x10, 0x14, 0x18};

  for (uint8_t i = 0; i < 6; i++) {
    MCP2515_write(dev, filt_bases[i] + 0, 0x00); // SIDH
    MCP2515_write(dev, filt_bases[i] + 1, 0x00); // SIDL
    MCP2515_write(dev, filt_bases[i] + 2, 0x00); // EID8
    MCP2515_write(dev, filt_bases[i] + 3, 0x00); // EID0
  }

  // Set RXB0CTRL and RXB1CTRL receive control register
  uint8_t rxb0 = 0x60; // receive all
  MCP2515_write(dev, 0x30, rxb0);
  uint8_t rxb1 = 0x60; // receive all
  MCP2515_write(dev, 0x40, rxb1);

  // Enable interrupts for (RX0IE|RX1IE)
  MCP2515_write(dev, 0x2B, 0x03);

  // OBS! This MUST be the final register we set. sets NORMAL mode, disables
  // ability to config

  // Comment this out if we want to communicate with other nodes
  MCP2515_bit_modify(dev, 0x0F, 0xE0,MODE_LOOPBACK); // Control regiser , mask, Loopback
  // Comment this back in if you comment out the line above
  // MCP2515_bit_modify(dev, 0x0F, 0xE0, MODE_NORMAL); // Control regiser ,
  // mask, normal mode
  uint8_t canstat;
  // Confirm that the controller has switched to normal mode
  MCP2515_read(dev, 0x0E, &canstat);
  if ((canstat & 0x0E) != MODE_LOOPBACK<<5) {
    printf("Failed to switch controller to normal mode when initialzing");
    return -3;
  }
  return 0;
}

// Write data to register beginning at selected address.
int8_t MCP2515_write(struct can_device *dev, uint8_t addr, uint8_t data) {
  
  unsigned char frame[3] = {MCP2515_OP_WRITE, addr, data};
  spi_send_n(&dev->spi, frame, 3);
  return 0;
}

// Read a specefic RX BUFFER see operation on pg 66 in CAN controller datasheet
int8_t MCP2515_read(struct can_device *dev, uint8_t addr, uint8_t *out) {

  // Make sure we are writing to somthing
  if (!*out) {
    return -1;
  }
  
  unsigned char tx[3] = {MCP2515_OP_READ, addr,0xff};
  unsigned char rx[3];
  spi_duplex(&dev->spi, tx, rx, 3);
  *out = rx[2];
	  
  return 0;
}

// Instructs controller to begin message transmission sequence for
// any of the transmit buffers.
int8_t MCP2515_request_to_send(struct can_device *dev, uint8_t tx_buf_num) {

  spi_send(&dev->spi, (MCP2515_OP_RTS_BASE | (1 << tx_buf_num)));

  return 0;
}

// Set or clear individual bits in a particular register
int8_t MCP2515_bit_modify(struct can_device *dev, uint8_t reg, uint8_t mask,
                          uint8_t set_val) {

  unsigned char cmd[4] = {MCP2515_OP_BIT_MODIFY, reg, mask, set_val};
  spi_send_n(&dev->spi, cmd, 4);

  return 0;
}

// Resets internal registers to default state, this sets Config mode
int8_t MCP2515_reset(struct can_device *dev) {

  spi_send(&dev->spi, MCP2515_OP_RESET);

  return 0;
}

// Quick polling command that reads several status bits for transmit and receive
// functions.
int8_t MCP2515_read_status(struct can_device *dev, uint8_t *out) {

  uint8_t *status;

  spi_push(&dev->spi, MCP2515_OP_READ_STATUS, NULL);
  spi_recieve(&dev->spi, out);

  return 0;
}
