#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include "spi.h"

//------------------//
//   GENERAL CAN    //
//------------------//

struct CAN_frame {

    uint32_t id;        // 11-bit, set extended == true for 29-bit adress
    uint8_t  dlc;       // 0-8 Data Length Code, How many databytes are in the message
    uint8_t  data[8];
    bool     extended;  // Decide ID size, true = 29-bit, false = 11-bit 
    bool     rtr;       // Remote Transmission Request, must be false for standard message 
                        // When rtr is true, asks a node with the same ID to respond with a data frame.
                        // On the MCP2515, the RTR bit is in the DLC register for TX/RX.
};


int8_t CAN_read(struct *dev,uint8_t address,uint8_t *out){

    MCP2515_read(dev, address, out);

    return 0;
}

int8_t CAN_write(struct *dev, address, uint8_t *data){
   
    MCP2515_write(dev,adress,data);

    return 0;
}
//------------------//
// MCP2515 Specific //
//------------------//


int8_t MCP2515_init(){
    uint8_t value ;

    while (spi_ready()) { // Confirm SPI Initialized
        status = spi_init();
    } 
    MCP2515_reset(); // Send reset - command

    // Confirm that we are in correct mode after boot, and that there are no pending messages.
    MCP2515_read_status(dev,&value);
    uint8_t mode= (0x7&value>>5);     // Extract code for mode from status
    uint8_t irq = (0x07&(&value>>1)); // Extract code for interrupt requests from status
    if (mode!= MODE_CONFIG){
        printf("SPI to CAN controller is not in configuration mode after reset!\n");
        return 1;
    }
    if(irq != 0){
        printf("There is an interrupt request when booting the SPI-CAN controllr")
        return 2;
    }

    uint8_t cnf1 = 0x00;
    uitn8_t cnf2 = 0xFF;
    uint8_t cnf3 = 0x03;

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
    const uint8_t filt_bases[6] = {
        0x00, 0x04, 0x08, 0x10, 0x14, 0x18
    };

    for (uint8_t i=0; i<6; i++){
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

    // OBS! This MUST be the final register we set. sets NORMAL mode, disables ability to config


    //Comment this out if we want to communicate with other nodes
    MCP2515_bit_modify(dev, 0x0F, 0xE0, MODE_LOOPBACK); // Control regiser , mask, Loopback
    //Comment this back in if you comment out the line above
    //MCP2515_bit_modify(dev, 0x0F, 0xE0, MODE_NORMAL); // Control regiser , mask, normal mode

    // Confirm that the controller has switched to normal mode
    MCP2515_read(dev, 0x0E, &canstat);
    if ((canstat & 0x0E) != MODE_LOOPBACK) {
        printf("Failed to switch controller to normal mode when initialzing");
        return -3;
    }
    return 0;
}

// Write data to register beginning at selected address.
int8_t MCP2515_write(*dev,uint8_t addr,uint8_t *data){
    
    CAN_CS &= ~(1 << CAN_CS_PIN);    // Set CS
    spi_push(dev, MCP2515_OP_WRITE); //Set operation
    spi_push(dev, addr);             // Set address
    spi_push(dev, data);             // Write data
    CAN_CS |=  (1 << CAN_CS_PIN);    // Reset CS
    return 0;
}

    return 0;
}

// Read a specefic RX BUFFER see operation on pg 66 in CAN controller datasheet
int8_t MCP2515_read(struct *dev,uint8_t addr , uint8_t *out){

    //Make sure we are writing to somthing
    if(!out) {
        return -1;
    }

    CAN_CS &= ~(1 << CAN_CS_PIN);    // Set CS
    spi_read(dev, MCP2515_OP_READ);  // Set operation
    spi_read(dev, addr);             // Set address
    *out = spi_read(dev, 0x00);      // Read data 
    CAN_CS |=  (1 << CAN_CS_PIN);    // Reset CS

    return 0;
}

// Instructs controller to begin message transmission sequence for
// any of the transmit buffers. 
int8_t MCP2515_request_to_send(struct *dev,uint8_t tx_buf_num){


    CAN_CS &= ~(1 << CAN_CS_PIN);    // Set CS
    spi_push(spi, MCP2515_OP_RTS_BASE | (1 << tx_buf_num));
    CAN_CS |=  (1 << CAN_CS_PIN);    // Reset CS

    return 0;
}


// Set or clear individual bits in a particular register
int8_t MCP2515_bit_modify(struct *dev, uint8_t reg, uint8_t mask, uint8_t set_val){

    CAN_CS &= ~(1 << CAN_CS_PIN);         // Set CS
    spi_txrx(dev, MCP2515_OP_BIT_MODIFY); //Set operation
    spi_txrx(dev, reg);                   //Set adress
    spi_txrx(dev, mask);                  //Set mask
    spi_txrx(dev, set_val);               //Set bits
    CAN_CS |=  (1 << CAN_CS_PIN);         // Reset CS

    return 0;
}

// Resets internal registers to default state, this sets Config mode
int8_t MCP2515_reset(struct *dev){

    spi_send(dev,0b11000000);

    return 0;
}

// Quick polling command that reads several status bits for transmit and receive functions.
int8_t MCP2515_read_status(struct *dev,uint8_t *out){

    uint8_t *status;
    spi_send(dev, 0b01010000);
    spi_recieve(dev, uint8_t out);

    return 0;
}