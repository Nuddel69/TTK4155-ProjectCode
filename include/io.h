#ifndef INCLUDE_INCLUDE_IO_H_
#define INCLUDE_INCLUDE_IO_H_

#include "spi.h"

struct io_oled_device {
  struct spi_interface spi;
  unsigned int ss;
};
struct io_joystick_device {
  unsigned int adc_channel_x;
  unsigned int adc_channel_y;
  int x_offset;
  int y_offset;
};
struct io_joystick_position {
  int x; // X value in % from the middle
  int y; // Y value in % from the middle
};
enum io_joystick_direction { LEFT, RIGHT, UP, DOWN, NEUTRAL };

//------------------//
//     Joystick     //
//------------------//

/*!
 * \brief Initialise and calibrate a joystick.
 * \param[in] dev The joystick to be initlaised.
 * \return Errno.
 */
int io_joystick_init(struct io_joystick_device *dev);

/*!
 * \brief Read the position of a joystick as a percentage from 0 in each axis.
 * \param[in] dev The joystick to read.
 * \param[in] buffer A buffer for storing the position value.
 * \return Errno.
 */
int io_joystick_read_position(struct io_joystick_device *dev,
                              struct io_joystick_position *buffer);

/*!
 * \brief Fetches the direction of a joystick.
 * \param[in] dev The joystick to read.
 * \param[in] direction A buffer for storing the read direction.
 * \return Errno.
 */
int io_joystick_read_direction(struct io_joystick_device *dev,
                               enum io_joystick_direction *direction);

/*!
 * \brief Calibrate a joystick. Requires the joystick to be in its neutral
 * position.
 * \param[in] dev The joystick to calibrate.
 * \return Errno.
 */
int io_joystick_calibrate(struct io_joystick_device *dev);

//------------------//
//       OLED       //
//------------------//

int io_oled_init(struct io_oled_device *dev);
int io_oled_reset(struct io_oled_device *dev);
int io_oled_home(struct io_oled_device *dev);
int io_oled_goto_line(struct io_oled_device *dev, int line);
int io_oled_goto_column(struct io_oled_device *dev, int column);
int io_oled_clear_line(struct io_oled_device *dev, int line);
int io_oled_pos(struct io_oled_device *dev, int row, int column);
int io_oled_print(struct io_oled_device *dev, char *text);
#endif // INCLUDE_INCLUDE_IO_H_
