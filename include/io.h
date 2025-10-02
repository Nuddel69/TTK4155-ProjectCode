#ifndef INCLUDE_INCLUDE_IO_H_
#define INCLUDE_INCLUDE_IO_H_

#include "spi.h"
#include "fonts.h"

struct oled_font {
  const void *table;        // pointer to font table
  uint8_t bytes_per_glyph;  // number of columns per glyph
  uint8_t height;           // pixel height
  uint8_t spacing;          // columns of space between glyphs
};

extern const unsigned char font8[95][8]  PROGMEM;
extern const unsigned char font5[95][5]  PROGMEM;
extern const unsigned char font4[95][4]  PROGMEM;

static const oled_font OLED_FONT_8x8 = { font8, 8, 8, 1 };
static const oled_font OLED_FONT_5x7 = { font5, 5, 7, 1 };
static const oled_font OLED_FONT_4x6 = { font4, 4, 6, 1 };

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

/*!
 * \brief Initialise OLED.
 * \param[in] dev The OLED to be initlaised.
 * \return Errno.
 */
int io_oled_init(struct io_oled_device *dev);

/*!
 * \brief Sends command to OLED
 * \param[in] dev The commanded OLED.
 * \param[in] command the hex command (see p27. SSD1309 datasheet)
 * \return Errno.
 */
int io_oled_cmd(struct io_oled_device *dev, unsigned int command);

/*!
 * \brief Sends data to OLED
 * \param[in] dev The OLED that should receive the data.
 * \param[in] data the data that the OLED should recive 
 * \return Errno.
 */
int io_oled_write(struct io_oled_device *dev, unsinged int data);

/*!
 * \brief Resets OLED to blank page
 * \param[in] dev The OLED to be reset.
 * \return Errno.
 */
int io_oled_reset(struct io_oled_device *dev);

/*!
 * \brief Loads homescreen to OLED
 * \param[in] dev The OLED to be set
 * \return Errno.
 */
int io_oled_home(struct io_oled_device *dev);

/*!
 * \brief Go to specific line in OLED
 * \param[in] dev The OLED who's line is to be set
 * \param[in] line the line to be set
 * \return Errno.
 */
int io_oled_goto_line(struct io_oled_device *dev, int line);

/*!
 * \brief Go to specific column in OLED
 * \param[in] dev The OLED who's column is to be set
 * \param[in] column the column which should be set
 * \return Errno.
 */
int io_oled_goto_column(struct io_oled_device *dev, int column);

/*!
 * \brief Clear specific line in OLED 
 * \param[in] dev The OLED who's line is to be cleared
 * \param[in] line the line that to be cleared
 * \return Errno.
 */
int io_oled_clear_line(struct io_oled_device *dev, int line);

/*!
 * \brief Writes data to OLED at current position
 * \param[in] dev The OLED where the data should be sent
 * \param[in] data The data to be sendt to the OLED
 * \return Errno.
 */
volatile int io_oled_writedata(struct io_oled_device *dev, int data);

/*!
 * \brief Set specific position in OLED
 * \param[in] dev The OLED who's position is to be set
 * \param[in] row The row of the position to be set
 * \param[in] collumn the collumn of the position the be set
 * \return Errno.
 */
int io_oled_pos(struct io_oled_device *dev, int row, int column);

/*!
 * \brief Write string to OLED
 * \param[in] dev The OLED who should print the line
 * \param[in] text the string that should be written to the OLED
 * \return Errno.
 */
int io_oled_print(struct io_oled_device *dev, char *text);

/*!
 * \brief Set OLED brightness
 * \param[in] dev The OLED who's britness should be set
 * \param[in] brightness the brightness level to be set
 * \return Errno.
 */
int io_oled_set_brightness(struct io_oled_device *dev, unsigned int brightness);

/*!
 * \brief Reset OLED brightness
 * \param[in] dev The OLED who's britness should be set
 * \return Errno.
 */
int io_oled_reset_brightness(struct io_oled_device *dev);

/*!
 * \brief Prints an arrow at a specified postion
 * \param[in] dev The OLED who's britness should be set
 * \param[in] row select row where the arrow should be placed
 * \param[in] column select column where the arrow should be placed
 * \return Errno.
 */
int io_oled_print_arrow (struct io_oled_device *dev, unsigned row , unsigned col);

#endif // INCLUDE_INCLUDE_IO_H_
