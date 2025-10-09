#ifndef INCLUDE_INCLUDE_SPI_H_
#define INCLUDE_INCLUDE_SPI_H_

enum spi_slave {
  SSB2,
  SSB4,
  SSE2,
};

int spi_init();
int spi_ready();
int spi_send(enum spi_slave *slave, unsigned char data);
int spi_recieve(enum spi_slave *slave, unsigned char *out);
int spi_duplex(enum spi_slave *slave, unsigned char data, unsigned char *out);
int spi_set_slave_select(enum spi_slave *slave, unsigned char state);

#endif // INCLUDE_INCLUDE_SPI_H_
