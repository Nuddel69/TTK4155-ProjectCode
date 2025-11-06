#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>

#include "can.h"
#include "spi.h"
#include "mcp.h"
 
 struct can_device *can_irq;
 struct CAN_frame new_message;
 
//------------------//
//   GENERAL CAN    //
//------------------//
int8_t can_init(struct can_device *dev) {
	
 can_irq = dev;
 struct CAN_frame new_message = {0x00,0x01,'1',0,0};

 // Set PE0 as input
 DDRE  &= ~(1<<PE0); 
 PORTE |=  (1<<PE0); 


 // Disable global interrupts
 cli();
 // Enable INT2
 GICR  |= (1<<CAN_INTERRUPT_ISR_REGISTER);

 // Edge select, active-low interrupt from MCP2515
 EMCUCR &= ~(1<<ISC2);

 // Configure interrupt falling edge
 //MCUCR |=(1<<ISC01);
 //MCUCR &= ~(1<<ISC00);
 // Enable global interrupts
 sei();
	    
 return MCP2515_init(dev);
}


int8_t can_write(struct can_device *dev, struct CAN_frame msg)
{
	uint8_t ID_MSB = (0x7F8 & msg.id) >> 3;
	uint8_t ID_LSB = (0x7 & msg.id) << 5;
	
	MCP2515_write(dev,MCP2515_TXB0SIDH, ID_MSB);
	MCP2515_write(dev,MCP2515_TXB0SIDL, ID_LSB);
	MCP2515_write(dev,TXB0DLC, msg.dlc);
	
	uint8_t buff0_status;
	MCP2515_read(dev,MCP2515_TXB0CTRL,&buff0_status);
	if ((buff0_status & 0x8) != 0x8) {
		if (msg.dlc > 8) {
      printf("TX Buffer0 overflow, message too large\r\n");
      return -2;
			
		}
		for(uint8_t i = 0; i < msg.dlc; i++)
		{	
			MCP2515_write(dev,MCP2515_TXB0D0+i,msg.data[i]);
		}
		
		MCP2515_request_to_send(dev,MCP2515_RTS_TX0);
		return 0;

	} else {
		printf("TX Buffer0 not avaliable\r\n");
		return -2;
	}
	
}

int8_t can_read(struct can_device *dev, struct CAN_frame *out) {
	

	uint8_t ID_MSB;
	uint8_t ID_LSB;
	uint8_t length;
	
	MCP2515_read(dev,MCP2515_RXB0SIDH,&ID_MSB);
	MCP2515_read(dev,MCP2515_RXB0SIDL,&ID_LSB);
		
	ID_LSB = (ID_LSB & 0xE0) >> 5;
	out->id = ID_MSB << 3;
	out->id = (out->id & 0x7F8) | (ID_LSB & 0x7);
		
	MCP2515_read(dev,MCP2515_RXB0DLC,&length);
	out->dlc = (length & 0xF);
	if (out->dlc > 8) {
		out->dlc = 8;
	}
		
	for (uint8_t i=0; i < out->dlc; i++) {
		MCP2515_read(dev,MCP2515_RXB0D0 + i,(uint8_t*)&out->data[i]);

    for (uint8_t i = 0; i < out->dlc; i++) {
    MCP2515_read(dev, MCP2515_RXB0D0 + i, &out->data[i]);
    printf("%02X ", out->data[i]);
    }
    printf("\r\n");
	}
	
	return 0;
}


/*
int8_t CAN_write(struct can_device *dev, uint8_t address,
                 struct CAN_frame data_frame) {

  // TXB0SIDH,
  MCP2515_write(dev, 0x31, data_frame.id);
  // TXB0SIDL
  MCP2515_write(dev, 0x32, (data_frame.id << 8));

  uint8_t DLC;
  // TXB0DLC
  if (data_frame.rtr) {
    DLC = (data_frame.dlc | (1 << 6));
  } else {
    DLC = data_frame.dlc;
  }

  // Set DLC 
  MCP2515_write(dev, 0x35, DLC);

  // Set data TXB0Dm Data going out from buffer 0
    MCP2515_write(dev, 0x36, data_frame.data);
  
  //Inform controller that we are ready to send on buffer0
  spi_send(&dev->spi,0x81);

  return 0;
}
*/

//------------------//
// MCP2515 Specific //
//------------------//

// External interrupt PE0, From MCP2515
ISR(INT2_vect) { 
	
	printf("External interrupt"); 
	uint8_t status;
	MCP2515_read_status(can_irq,&status);
	
	if (status & rx_buff_0_full) {
		can_read(can_irq, &new_message);
		printf("RX0 Full\r\n");
	}
	
	if (status & rx_buff_1_full) {
		can_read(can_irq, &new_message);
		printf("RX1 Full\r\n");
	}
	
	if (status & tx_buff_0_busy) {
		printf("TX0 Busy\r\n");
	}
	
	if (status & tx_buff_0_empty) {
		printf("TX0 Empty\r\n");
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
  MCP2515_read(dev,0x0E,&value);
  

  //Ensure config mode after reset
  if ((value&MODE_MASK)!=(MODE_CONFIG)) {
    printf("SPI to CAN controller is not in configuration mode after reset!\n");
    return -1;
  }

  //MCP2515_read_status(dev, &value);
  if ((value&0x0E)!=(MCP2515_NO_IRQ)) {
    printf("There is an interrupt request when booting the SPI-CAN controller");
    return -2;
  }

  //RTS control
  unsigned char tx[3] = {MCP2515_WRITE, MCP2515_TXRTSCTRL,MCP2515_TXRTS_CONF};
  spi_send_n(&dev->spi,tx,3);  
	 
	 // Receive buffer0 config. Receive all, no overflow
	 MCP2515_bit_modify(dev,MCP2515_RXB0CTRL, 0x60, 0x60);
	 MCP2515_read(dev,MCP2515_RXB0CTRL,&value);
	 if (value != 0x60){
		 printf("Couldnt configure MCP2515 RX0 Buffer\r\n");
		 return -3;
	 }
	 
	 // Receive buffer1 config. Receive all
	 MCP2515_bit_modify(dev,MCP2515_RXB1CTRL, 0x60, 0x60);
	 MCP2515_read(dev,MCP2515_RXB1CTRL,&value);
	 if (value != 0x60){
		 printf("Couldnt configure MCP2515 RX1\r\n");
		 return -4;
	 }	

  // Set PE0 as input
  DDRE  &= ~(1<<PE0); 
  PORTE |=  (1<<PE0); 

  // Enable INT2
  GICR  |= (1<<INT2);

  // Edge select, active-low interrupt from MCP2515
  EMCUCR &= ~(1<<ISC2);


  // Interrupt config: msg error, error flag change, TX0 empty, RX0 full
	MCP2515_bit_modify(dev,MCP2515_CANINTE, 0xFF, 0x5);
	MCP2515_read(dev,MCP2515_CANINTE,&value);
	if (value != 0x5){
		printf("Couldnt set IRQ config for MCP2515\r\n");
		return -5;
	} 

  //BRP and CAN timing config
  MCP2515_write(dev, MCP2515_CNF1, MCP2515_VAL_CNF1);
  MCP2515_write(dev, MCP2515_CNF2, MCP2515_VAL_CNF2);
  MCP2515_write(dev, MCP2515_CNF3, MCP2515_VAL_CNF3);

	MCP2515_read(dev,MCP2515_CNF1,&value);
	if (value != MCP2515_VAL_CNF1){
		printf("Couldnt configure BRP for MCP2515\r\n");
		return -6;
	}

	MCP2515_read(dev,MCP2515_CNF2,&value);
	if (value != MCP2515_VAL_CNF2){
		printf(" CNF2 not set\r\n");
		return -7;
	}

	MCP2515_read(dev,MCP2515_CNF3,&value);
	if (value != MCP2515_VAL_CNF3){
		printf(" CNF3 not set\r\n");
		return -8;
	}

  //Set the controller to normal mode !!!THIS HAS TO BE LAST IN INIT!!!
    MCP2515_write(dev,MCP2515_CANCTRL, MODE_NORMAL);
	MCP2515_read(dev,MCP2515_CANSTAT,&value);
	if ((value & MODE_MASK) != MODE_NORMAL){
		printf("MCP2515 could not be set to NORMAL mode when init compelte \r\n");
		return -9;
	 }

  /*

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

  */

  // OBS! This MUST be the final register we set. sets NORMAL mode, disables
  // ability to config

  // Comment this out if we want to communicate with other nodes
  // MCP2515_bit_modify(dev, 0x0F, 0x70, 0x40); // Control regiser , mask, Loopback
  // Comment this back in if you comment out the line above
  // MCP2515_bit_modify(dev, 0x0F, 0xE0, MODE_NORMAL); // Control regiser ,
  // mask, normal mode
  // Confirm that the controller has switched to normal mode
  
  /* MCP2515_read(dev, 0x0E, &value);
  if ((value & 0xE0) != MODE_LOOPBACK<<5) {
    printf("Failed to switch controller to normal mode when initialzing");
    return -3;
  }
  */

  return 0;
}

// Write data to register beginning at selected address.
int8_t MCP2515_write(struct can_device *dev, uint8_t addr, uint8_t data) {
  
  unsigned char frame[3] = {MCP2515_WRITE, addr, data};
  spi_send_n(&dev->spi, frame, 3);
  
  return 0;
}

int8_t MCP2515_write_n(struct can_device *dev, uint8_t addr, uint8_t *data) {
  
  unsigned char frame = {MCP2515_WRITE, addr, data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7]};
  spi_send_n(&dev->spi, frame, 2+8);
  
  return 0;
}

// Read a specefic RX BUFFER see operation on pg 66 in CAN controller datasheet
int8_t MCP2515_read(struct can_device *dev, uint8_t addr, uint8_t *out) {

  // Make sure we are writing to somthing
  if (!out) {
    return -1;
  }
  
  unsigned char tx[3] = {MCP2515_READ, addr,0xff};
  unsigned char rx[3];
  //spi_duplex(&dev->spi, tx, rx, 3);
  spi_push(&dev->spi,tx[0],NULL);
  spi_push(&dev->spi,tx[1],NULL);
  spi_recieve(&dev->spi,out);
	  
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
  spi_duplex(&dev->spi,cmd,NULL,4);
  //spi_send_n(&dev->spi, cmd, 4);

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
