#include <avr/io.h>
#include <errno.h>
#include <stdio.h>
#include <util/delay.h>

#include "spi.h"
#include "utils.h"

int spi_init(struct spi_interface *slave) {
  printf("Start SPI init \r\n");
  DDRB |= (1 << DDB5) | (1 << DDB7) | (1 << DDB2) |
          (1 << DDB4); // Sets the required PORT B pins to SPI mode

  SPCR |= (1 << SPE) 
        |(1 << MSTR) // Enables the SPI-bus and sets it to master mode, MSB
        |(1<<SPR0);  // first, no interrupt, and a clock freq of f_osc/4

  spi_set_slave_select(0, 1);
  printf("Complete SPI init \r\n");
  return 0;
}

int spi_ready(struct spi_interface *slave) { return 0; }

int spi_set_slave_select(int slave_pin, unsigned char state) {
  if (!state) {
    PORTB &= ~(1 << PB2);
  } else {
    PORTB |= (1 << PB2);
  }
  return 0;
}

int spi_send(struct spi_interface *slave, unsigned char data) {
  spi_duplex(slave, data, NULL);
  return 0;
}

int spi_recieve(struct spi_interface *slave, unsigned char *out) {
  *out = SPDR;
  return 0;
}

int spi_duplex(struct spi_interface *slave, unsigned char data,
               unsigned char *out) {
  spi_set_slave_select(0, 0);
  SPDR = data;
    //printf("Starting sending data over SPI\r\n");
  while (!(SPSR & (1 << SPIF))) {
    //printf("Waiting for SPI transmission complete flag\r\n");
  }
  *out = SPDR;
  spi_set_slave_select(0, 1);
  return 0;
}
