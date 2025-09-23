#include <errno.h>

#include "adc.h"
#include "io.h"

int io_oled_init(struct oled_device *dev, int *status);

int io_joystick_read(struct joystick_device *dev, int *buffer) {
  return -ENOERR;
}
int io_joystick_calibrate(struct joystick_device *dev) { return -ENOERR; }
