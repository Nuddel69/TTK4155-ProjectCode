#include <avr/io.h>
#include <errno.h>
#include <util/delay.h>

#include "spi.h"
#include "utils.h"

int spi_init(struct spi_interface *slave) {
  DDRB |=
        (1 << DDB5) |
        (1 << DDB7) | (1 << DDB2); // Sets the required PORT B pins to SPI mode

  SPCR |= (1 << SPE) |
          (1 << MSTR); // Enables the SPI-bus and sets it to master mode, MSB
                       // first, no interrupt, and a clock freq of f_osc/4

  spi_set_slave_select(0, 1);
  return 0;
}

int spi_ready(struct spi_interface *slave) { return 0; }

int spi_set_slave_select(int slave_pin, unsigned char state) {
  if (!state) {
    printf("start setting slave low\n\r");
    PORTB &= ~(1 << PB2);
    printf("done setting low\n\r");
  } else {
    printf("start setting slave high\n\r");
    PORTB |= (1 << PB2);
    printf("done setting slave low\n\r");
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
  printf("Enter while\n\r");
  while (!(SPSR & (1 << SPIF)))
    ;
    printf("Done while\n\r");
  *out = SPDR;
  spi_set_slave_select(0, 1);
  return 0;
}
