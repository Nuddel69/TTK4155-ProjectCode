#ifndef INCLUDE_INCLUDE_SPI_H_
#define INCLUDE_INCLUDE_SPI_H_

struct spi_interface {
  unsigned int mosi;
  unsigned int miso;
  unsigned int clk;
};

int spi_init(struct spi_interface *dev);

#endif // INCLUDE_INCLUDE_SPI_H_
