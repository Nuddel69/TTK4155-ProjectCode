#ifndef INCLUDE_INCLUDE_IO_H_
#define INCLUDE_INCLUDE_IO_H_

#include "spi.h"

struct oled_device {
  struct spi_interface spi;
  unsigned int cs;
};

struct joystick_device {
  unsigned int adc_address; // Dunno what's needed, placeholder field
};

int io_oled_init(struct oled_device *dev, int *status);

int io_joystick_init(struct joystick_device *dev);
int io_joystick_read(struct joystick_device *dev, int *buffer);
int io_joystick_calibrate(struct joystick_device *dev);

#endif // INCLUDE_INCLUDE_IO_H_
