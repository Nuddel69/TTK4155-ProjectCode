#include <errno.h>

#include "adc.h"
#include "io.h"

#define JOYSTICK_THRESHOLD 10

//------------------//
//     Joystick     //
//------------------//

int io_joystick_init(struct io_joystick_device *dev) {
  int status = 0;

  // if (adc_ready()) {
  //   status = adc_init();
  // }

  status = io_joystick_calibrate(dev);

  return status;
}
int io_joystick_read_position(struct io_joystick_device *dev,
                              struct io_joystick_position *buffer) {
  return 0;
}
int io_joystick_read_direction(struct io_joystick_device *dev,
                               enum io_joystick_direction *direction) {
  int status = 0;

  struct io_joystick_position pos;
  status = io_joystick_read_position(dev, &pos);
  if (status) {
    return status;
  }

  if (pos.x > (0 - JOYSTICK_THRESHOLD) && pos.x < (0 + JOYSTICK_THRESHOLD) &&
      pos.y > (0 - JOYSTICK_THRESHOLD) && pos.y < (0 + JOYSTICK_THRESHOLD)) {
    *direction = NEUTRAL;
    return status;
  }

  // Fill inn the remaining directions...

  return status;
}
int io_joystick_calibrate(struct io_joystick_device *dev) {
  int status = 0;

  struct io_joystick_position pos;
  status = io_joystick_read_position(dev, &pos);
  if (status) {
    return status;
  }

  dev->x_offset = pos.x * -1;
  dev->y_offset = pos.y * -1;

  return 0;
}

//------------------//
//       OLED       //
//------------------//

int io_oled_init(struct io_oled_device *dev) {
  int status = 0;

  while (spi_ready(&dev->spi)) {
    status = spi_init(&dev->spi);
  }

  /*/
   *
   * OLED STUFF
   *
  /*/

  return status;
}
int io_oled_reset(struct io_oled_device *dev) { return 0; }
int io_oled_home(struct io_oled_device *dev) { return 0; }
int io_oled_goto_line(struct io_oled_device *dev, int line) { return 0; }
int io_oled_goto_column(struct io_oled_device *dev, int column) { return 0; }
int io_oled_clear_line(struct io_oled_device *dev, int line) { return 0; }
int io_oled_pos(struct io_oled_device *dev, int row, int column) { return 0; }
int io_oled_print(struct io_oled_device *dev, char *text) { return 0; }
