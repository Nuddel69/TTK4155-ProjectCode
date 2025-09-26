#ifndef INCLUDE_INCLUDE_SPI_H_
#define INCLUDE_INCLUDE_SPI_H_

struct spi_interface{
  unsigned int mosi;
  unsigned int miso;
  unsigned int clk;
};

int spi_init(struct spi_interface *dev);
int spi_ready(struct spi_interface *dev);
int spi_send(struct spi_interface *slave, unsigned char data);
int spi_recieve(struct spi_interface *slave, unsigned char *out);
int spi_duplex(struct spi_interface *slave, unsigned char data,unsigned char *out);
int spi_set_slave_select(int slave_pin, unsigned char state);

#endif // INCLUDE_INCLUDE_SPI_H_
