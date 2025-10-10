#include <avr/io.h>
#include <stdint.h>
#include "spi.h"

//------------------//
//   GENERAL CAN    //
//------------------//


int8_t CAN_init(struct *dev){

return 0;
}

int8_t CAN_read(struct *dev,uint8_t address){

    uint8_t result;
    PORTB &= ~(1<<CAN_CS);             // Select CAN 
    spi_send(dev,MCP_READ);            // Send read instruction
    spi_send(dev,address);             // Send address
    spi_recieve(spi_slave,adress);     // Read result
    PORTB |= (1<<CAN_CS);              // Deselect CAN 
    return 0;
}

//------------------//
// MCP2515 Specific //
//------------------//


int8_t init_MSCP2515(){
    uint8_t value ;

    while (spi_ready()) { // Confirm SPI Initialized
        status = spi_init();
    } 
    MCP2515_reset(); // Send reset - command

    // Confirm that we are in correct mode after boot
    MCP2515_read_status(dev,&value);
    uint8_t mode= (0x7&value>>5);     // Extract Mode from status
    uint8_t irq = (0x07&(&value>>1)); // Extract Interrupt requests from status
    if (mode!= MODE_CONFIG){
        printf("MCP2515 is NOT in configuration mode after reset !\n");
        return 1;
    }
    if(irq != 0){
        printf("There is an interrupt request when booting the CAN")
        return 2;
    }
    //Set Config
    MCP2515_bit_modify(dev,);
    //Set RX Masks
    MCP2515_bit_modify(dev,);

    
    //REST OF CAN INIT GOES HERE



    return 0;
}

// Write data to register beginning at selected address.
int8_t MCP2515_write(*dev,uint8_t *data){

    spi_send(dev,0b00000010);      //
    spi_send(dev,);
    MCP2515_request_to_send(dev,1); 

    return 0;
}

// Read a specefic RX BUFFER see command at pg 66 in CAN controller datasheet
int8_t MCP2515_read(struct *dev,uint8_t rx_buf_num, uint8_t *out){

    uint8_t cmd = (0b10010000|(rx_buf_num<<1))
    spi_send(dev,cmd);
    spi_recieve(dev, uint8_t out);

    return 0;
}

// Instructs controller to begin message transmission sequence for
// any of the transmit buffers. 
int8_t MCP2515_request_to_send(struct *dev,uint8_t tx_buf_num){

    uint_t cmd = (0b10000000|tx_buf_num);
    spi_send(dev,cmd);

    return 0;
}


// Set or clear individual bits in a particular register
int8_t MCP2515_bit_modify(struct *dev, uint8_t reg, uint8_t set_val){

    spi_send(dev,0b00000101);
    spi_send(dev,reg);
    spi_send(dev,set_val);

    return 0;
}

// Resets internal registers to default state, set Configuration mode
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