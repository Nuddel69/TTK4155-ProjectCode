#include <avr/io.h>
#include <util/delay.h>

#include "spi.h"
#include "utils.h"

static unsigned char _ready = 0;

int spi_init() {
  int status = 0;

  DDRB |= (1 << PB5) | (1 << PB7) |
          (1 << PB4); // Sets the required PORT B pins to SPI mode

  SPCR |= (1 << SPE) |
          (1 << MSTR)    // Enables the SPI-bus and sets it to master mode, MSB
          | (1 << SPR0); // first, no interrupt, and a clock freq of f_osc/4

  /*
  // Define PB2, PB4 and PE2 as slave select pins
  */
  DDRB |= (1 << PB2) | (1 << PB4);
  DDRE |= (1 << PE2);

  /*
  // Ensure initial state off
  */
  status = spi_set_slave_select((enum spi_slave *)SSB2, 1);
  status = spi_set_slave_select((enum spi_slave *)SSB4, 1);
  status = spi_set_slave_select((enum spi_slave *)SSE2, 1);

  if (status) {
    return status;
  }

  _ready = 1;

  return status;
}

int spi_ready() { return !_ready; }

int spi_set_slave_select(enum spi_slave *slave, unsigned char state) {
  switch (*slave) {
  case SSB2:
    if (!state) {
      PORTB &= ~(1 << PB2);
    } else {
      PORTB |= (1 << PB2);
    }
    break;
  case SSB4:
    if (!state) {
      PORTB &= ~(1 << PB4);
    } else {
      PORTB |= (1 << PB4);
    }
    break;
  case SSE2:
    if (!state) {
      PORTE &= ~(1 << PE2);
    } else {
      PORTE |= (1 << PE2);
    }
    break;
  default:
    return -1;
  }
  return 0;
}

int spi_send(enum spi_slave *slave, unsigned char data) {
  spi_duplex(slave, data, NULL);
  return 0;
}

int spi_recieve(enum spi_slave *slave, unsigned char *out) {
  *out = SPDR;
  return 0;
}

int spi_duplex(enum spi_slave *slave, unsigned char data, unsigned char *out) {
  spi_set_slave_select(slave, 0);
  SPDR = data;
  while (!(SPSR & (1 << SPIF))) {
  }
  *out = SPDR;
  spi_set_slave_select(slave, 1);
  return 0;
}
