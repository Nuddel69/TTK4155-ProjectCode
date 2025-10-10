
#include <avr/io.h>
#include <stdint.h>
#include "spi.h"


int8_t CAN_init(struct *dev){

uint8_t value ;
SPI_init(); // Initialize SPI
CAN_reset(); // Send reset - command
// Self - test
CAN_read(MCP_CANSTAT,&value);
if ((value&MODE_MASK)!=MODE_CONFIG){
    printf("MCP2515 is NOT in configuration mode after reset !\n");
    return 1;
    }
return 0;
}

//Taken directly from lab lecture
int8_t CAN_read(struct *dev,uint8_t address){

    uint8_t result;
    PORTB &= ~(1<<CAN_CS);                 // Select CAN - controller  -- Not sure if this is correct pin for us
    spi_send(dev,MCP_READ);                // Send read instruction
    spi_send(dev,address);                 // Send address
    pi_recieve(spi_slave,adress);          // Read result
    PORTB |= (1<<CAN_CS);                  // Deselect CAN - controller -- Not sure if this is correct pin for us
    return 0;
}

int8_t CAN_write(){

}

int8_t CAN_request_to_send(){

}

int8_t CAN_bit_modify(){

}
int8_t CAN_reset(){

}
int8_t CAN_read_status(){

}