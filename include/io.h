#ifndef INCLUDE_INCLUDE_IO_H_
#define INCLUDE_INCLUDE_IO_H_

#include "fonts.h"
#include "spi.h"
#include <stdint.h>

struct oled_font {
  const void *table;       // pointer to font table
  uint8_t bytes_per_glyph; // number of columns per glyph
  uint8_t height;          // pixel height
  uint8_t spacing;         // columns of space between glyphs
};

static const struct oled_font OLED_FONT_8x8 = {font8, 8, 8, 1};
static const struct oled_font OLED_FONT_5x7 = {font5, 5, 7, 1};
static const struct oled_font OLED_FONT_4x6 = {font4, 4, 6, 1};

struct io_avr_device {
  enum spi_slave spi;
};

struct io_oled_device {
  enum spi_slave spi;
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

struct __attribute__((packed)) io_avr_buttons {
  union {
    uint8_t right;
    struct {
      uint8_t R1 : 1;
      uint8_t R2 : 1;
      uint8_t R3 : 1;
      uint8_t R4 : 1;
      uint8_t R5 : 1;
      uint8_t R6 : 1;
    };
  };
  union {
    uint8_t left;
    struct {
      uint8_t L1 : 1;
      uint8_t L2 : 1;
      uint8_t L3 : 1;
      uint8_t L4 : 1;
      uint8_t L5 : 1;
      uint8_t L6 : 1;
      uint8_t L7 : 1;
    };
  };
  union {
    uint8_t nav;
    struct {
      uint8_t NB : 1;
      uint8_t NR : 1;
      uint8_t ND : 1;
      uint8_t NL : 1;
      uint8_t NU : 1;
    };
  };
};

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

//-------------------//
//   Button Inputs   //
//-------------------//

int io_avr_init(struct io_avr_device *dev);
int io_avr_buttons_read(struct io_avr_device *dev, struct io_avr_buttons *btn);
int io_avr_led_set(struct io_avr_device *dev, unsigned char led,
                   unsigned char brightness);

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
// int io_oled_cmd(struct io_oled_device *dev, uint8_t command);

/*!
 * \brief Sends data to OLED
 * \param[in] dev The OLED that should receive the data.
 * \param[in] data the data that the OLED should recive
 * \return Errno.
 */
// int io_oled_write(struct io_oled_device *dev, uint8_t data);

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
int io_oled_set_brightness(struct io_oled_device *dev, uint8_t brightness);

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
int io_oled_print_arrow(struct io_oled_device *dev, uint8_t row, uint8_t col);

/*!
 * \brief Prints a single glyph in a specifiec font
 * \param[in] dev the OLED that should be written to
 * \param[in] font the specified font, see font.h
 * \param[in] character_id the character that should be written
 * \return Errno.
 */
uint8_t io_oled_write_glyph(struct io_oled_device *dev,
                            const struct oled_font *font, char character_id);

/*!
 * \brief Prints an arrow at a specified postion
 * \param[in] dev the OLED device that should be written to
 * \param[in] font the specified font, see font.h
 * \param[in] text the string that should be printed to the OLED, has to end in
 * newline
 * \return .
 */
int io_oled_print_with_font(struct io_oled_device *dev,
                            const struct oled_font *font, char *text);

/*!
 * \brief Prints a known image to the OLED to confirm that it can print and
 * navigate the screen
 * \param[in] dev the OLED device that should be tested
 * newline
 * \return .
 */
int io_oled_test(struct io_oled_device *dev);

/*!
 * \brief Alternates screen between black and white
 * \param[in] dev the OLED device that should be blinked
 * \param[in] blinks number of blinks
 * newline
 * \return .
 */
int io_oled_blinky(struct io_oled_device *dev, uint8_t blinks);

#endif // INCLUDE_INCLUDE_IO_H_
