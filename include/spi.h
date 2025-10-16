#ifndef INCLUDE_INCLUDE_SPI_H_
#define INCLUDE_INCLUDE_SPI_H_

enum spi_slave {
  SSB2,
  SSB3,
  SSE2,
};

int spi_init();
int spi_ready();
int spi_send_n(enum spi_slave *slave, unsigned char *data, int length);
int spi_send(enum spi_slave *slave, unsigned char data);
int spi_recieve(enum spi_slave *slave, unsigned char *out);
int spi_recieve_n(enum spi_slave *slave, unsigned char *out, int length);
int spi_duplex(enum spi_slave *slave, unsigned char *data, unsigned char *out,
               int length);
int spi_set_slave_select(enum spi_slave *slave, unsigned char state);
int spi_push(enum spi_slave *slave, unsigned char data, unsigned char *out);

#endif // INCLUDE_INCLUDE_SPI_H_
