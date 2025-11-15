#include <avr/pgmspace.h>
#include <stdint.h>
#include <util/delay.h>

#include "adc.h"
#include "can.h"
#include "io.h"
#include "log.h"
#include "spi.h"
#include "uart.h"
#include "utils.h"
#include <stdio.h>

LOG_MODULE_DEFINE("IO")

#define JOYSTICK_THRESHOLD 10

#define IO_AVR_TOUCH_PAD 0x01
#define IO_AVR_TOUCH_SLIDER 0x02
#define IO_AVR_JOYSTICK 0x03
#define IO_AVR_BUTTONS 0x04
#define IO_AVR_LED_TOGGLE 0x05
#define IO_AVR_LED_PWM 0x06
#define IO_AVR_INFO 0x07

//------------------//
//     Joystick     //
//------------------//

int io_joystick_init(struct io_joystick_device *dev) {
  int status = 0;

  ADC_init();

  return status;
}

int io_joystick_read_position(struct io_joystick_device *dev,
                              struct io_joystick_position *buffer) {
  struct ADC_meas data;

  ADC_read_all(&data);

  uint8_t raw_x = data.channel[dev->adc_channel_x];
  uint8_t raw_y = data.channel[dev->adc_channel_y];

  if (raw_x < dev->x_min)
    raw_x = dev->x_min;
  if (raw_x > dev->x_max)
    raw_x = dev->x_max;
  if (raw_y < dev->y_min)
    raw_y = dev->y_min;
  if (raw_y > dev->y_max)
    raw_y = dev->y_max;

  buffer->x = map(raw_x, dev->x_min, dev->x_max, -127, 127);
  buffer->y = map(raw_y, dev->y_min, dev->y_max, -127, 127);

  if (buffer->x > -5 && buffer->x < 5)
    buffer->x = 0;
  if (buffer->y > -5 && buffer->y < 5)
    buffer->y = 0;

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

// int io_joystick_calibrate(struct io_joystick_device *dev) {
//   int status = 0;
//
//   struct io_joystick_position pos;
//   status = io_joystick_read_position(dev, &pos);
//   if (status) {
//     return status;
//   }
//
//   dev->x_offset = pos.x * -1;
//   dev->y_offset = pos.y * -1;
//
//   return 0;
// }

//-------------------//
//   Button Inputs   //
//-------------------//

int io_avr_init(struct io_avr_device *dev) {

  int status = 0;

  while (spi_ready()) {
    status = spi_init();
  }
  return status;
}
int io_avr_buttons_read(struct io_avr_device *dev, struct io_avr_buttons *btn) {

  unsigned char recbuf[3];
  spi_push(&dev->spi, IO_AVR_BUTTONS, NULL);
  spi_recieve_n(&dev->spi, recbuf, 3);
  spi_set_slave_select(&dev->spi, 1);

  btn->right = recbuf[0];
  btn->left = recbuf[1];
  btn->nav = recbuf[2];

  return 0;
}
int io_avr_led_set(struct io_avr_device *dev, unsigned char led,
                   unsigned char brightness) {
  unsigned char cmd[3] = {IO_AVR_LED_PWM, led, brightness};
  spi_send_n(&dev->spi, cmd, 3);
  return 0;
}

//------------------//
//       OLED       //
//------------------//

//-----------Low level OLED---------------

static int _io_oled_cmd(struct io_oled_device *dev, uint8_t command) {
  PORTB &= ~(1 << PB1); // D/!C=0
                        // PB2 DISP_CS Low, Chipselect is driven by SPI
  spi_send(&dev->spi, command);
  //_delay_us(3);
  return 0;
}

static int _io_oled_write(struct io_oled_device *dev, uint8_t data) {
  PORTB |= (1 << PB1); // D/!C set high
                       // PB2 DISP_CS Low, Chipselect is driven by SPI
  spi_send(&dev->spi, data);
  //_delay_us(3);
  dev->current_column++;

  return 0;
}

//---------------OLED commands-----------

int io_oled_init(struct io_oled_device *dev) {
  int status = 0;

  while (spi_ready()) {
    status = spi_init();
  }

  // --- Configure OLED control pins --- //
  // Set directions *after* setting desired logic states to avoid glitching
  DDRB |= (1 << PB4) | (1 << PB1) | (1 << PB2); // Set PB0,PB1,PB2 to outputs

  // --- Hardware reset sequence --- //
  PORTB &= ~(1 << PB4);
  _delay_us(5);
  PORTB |= (1 << PB4);
  _delay_ms(1);

  // Display OFF
  _io_oled_cmd(dev, 0xAE);

  // Recommended config for 128x64 OLED, page 19 OLED datasheet
  _io_oled_cmd(dev, 0xA1); // Segment remap: column address 127->0
  _io_oled_cmd(dev, 0xC8); // Scan direction: remapped C8 or C0

  _io_oled_cmd(dev, 0xDA); // Common pads hardware
  _io_oled_cmd(dev, 0x12); // Required parameter for 128x64

  _io_oled_cmd(dev, 0xC8); // Common output scan direciton

  _io_oled_cmd(dev, 0xA8); // Set multiplex ratio
  _io_oled_cmd(dev, 0x3F); // Multiplex ratio: 63 (64MUX)

  _io_oled_cmd(dev, 0xD5); // set display clock devider ratio
  _io_oled_cmd(dev, 0x80); // freq.mode

  _io_oled_cmd(dev, 0x81); // Contrast control
  _io_oled_cmd(dev, 0xFF);

  _io_oled_cmd(dev, 0xD9); // Set precharged period
  _io_oled_cmd(dev, 0x21);

  _io_oled_cmd(dev, 0x20); // Set memory adressing mode
  _io_oled_cmd(dev, 0x02); // 0x02 = Page (alternativly 0x00 = Horizontal)

  _io_oled_cmd(dev, 0xDB); // VCOM deselect level mode
  _io_oled_cmd(dev, 0x30);

  _io_oled_cmd(dev, 0xAD); // Master config
  _io_oled_cmd(dev, 0x00); // Slave config

  _io_oled_cmd(dev, 0xA4); // Resume to RAM content

  _io_oled_cmd(dev, 0xA6); // Normal display

  // Clear GDDRAM once at init
  for (uint8_t page = 0; page < 8; page++) {
    io_oled_goto_line(dev, page);
    io_oled_goto_column(dev, 0);
    for (uint8_t x = 0; x < 128; x++) {
      _io_oled_write(dev, 0x00);
    }
  }

  // Display ON
  _io_oled_cmd(dev, 0xAF);

  return status;
}

int io_oled_reset(struct io_oled_device *dev) {

  // Display off
  _io_oled_cmd(dev, 0xAE);

  // --- Hardware reset sequence --- //
  PORTB &= ~(1 << PB4); // Set PB4 low
  _delay_us(5);         // Wait 5us
  PORTB |= (1 << PB4);  // Set PB4 high
  _delay_ms(1); // Making sure its restarted before we start sending commands

  // Turn display on
  _io_oled_cmd(dev, 0xAF);

  return 0;
}

int io_oled_home(struct io_oled_device *dev) {

  io_oled_goto_line(dev, 0);
  io_oled_goto_column(dev, 0);

  return 0;
}

int io_oled_goto_line(struct io_oled_device *dev, int line) {

  if (line > 7)
    line = 7;                                 // Out of bounds
  _io_oled_cmd(dev, 0xB0 | (line & 0x07));    // 0xB0-0xB7
  dev->current_page = line;                   // Update index of current page

  return 0;
}

int io_oled_goto_column(struct io_oled_device *dev, int column) {

  // Columns are split into low/high area commands 0x00-0x0F and 0x10-0x1F
  _io_oled_cmd(dev, 0x00 | (column & 0x0F));        // lower
  _io_oled_cmd(dev, 0x10 | ((column >> 4) & 0x0F)); // higher
  dev->current_column = column;                     // Update index of current column

  return 0;
}

int io_oled_clear_line(struct io_oled_device *dev, int line) {

  if (line < 0 || line > 7)
    return -1;
  io_oled_goto_line(dev, line);
  io_oled_goto_column(dev, 0);
  for (uint8_t x = 0; x < 128; x++) {
    _io_oled_write(dev, 0x00);
  }
  return 0;
}

/**
 * \brief Clears all lines on the OLED (fills entire display with black)
 * \param[in] dev Pointer to the OLED device struct
 * \return Errno (0 = success, -1 = failure)
 */
int io_oled_clear_all(struct io_oled_device *dev) {
  int status = 0;

  // There are 8 pages (0–7) on a 64-pixel-tall display
  for (uint8_t line = 0; line < 8; line++) {
    status = io_oled_clear_line(dev, line);
    if (status < 0) {
      return status; // early exit on failure
    }
  }

  return 0;
}

int io_oled_pos(struct io_oled_device *dev, int line, int column) {

  io_oled_goto_line(dev, line);
  io_oled_goto_column(dev, column);

  return 0;
}

int io_oled_set_brightness(struct io_oled_device *dev, uint8_t brightness) {

  _io_oled_cmd(dev, 0x81);
  _io_oled_cmd(dev, brightness);

  return 0;
}

int io_oled_reset_brightness(struct io_oled_device *dev) {

  _io_oled_cmd(dev, 0x81);
  _io_oled_cmd(dev, 0x7F);

  return 0;
}

int io_oled_print_arrow(struct io_oled_device *dev, uint8_t row, uint8_t col) {

  io_oled_pos(dev, row, col);
  _io_oled_write(dev, 0b00011000);
  _io_oled_write(dev, 0b00011000);
  _io_oled_write(dev, 0b01111110);
  _io_oled_write(dev, 0b00111100);
  _io_oled_write(dev, 0b00011000);

  return 0;
}

// Write one glyph from specified font in font.h (ASCII 32..126).
uint8_t io_oled_write_glyph(struct io_oled_device *dev,
                            const struct oled_font *font, char character_id) {

  uint8_t start_page = dev->current_page;
  uint8_t start_column = dev->current_column;

  // --- Draw the glyph on the current page --- //
  uint8_t glyph_id = 0; // Space as default value

  // Make sure char_id in range
  if ((uint8_t)character_id >= 32 && (uint8_t)character_id <= 126) {
    glyph_id = (uint8_t)character_id - 32;
  }

  // Find glyph in fonts.h
  uint8_t *base = (uint8_t *)font->table + (glyph_id * font->bytes_per_glyph);

  // Print glyph to oled
  for (uint8_t c = 0; c < font->bytes_per_glyph; ++c) {
    // function from <avr/pgmspace.h> that reads from PROGMEM
    uint8_t col = pgm_read_byte(base + c);
    _io_oled_write(dev, col);
  }

  // Add spacing after glyph
  for (uint8_t s = 0; s < font->spacing; ++s) {
    _io_oled_write(dev, 0x00);
  }

  uint8_t total_cols = (uint8_t)(font->bytes_per_glyph + font->spacing);

  return total_cols;
}

int io_oled_print_with_font(struct io_oled_device *dev,
                            const struct oled_font *font, char *text) {
  if (!dev || !font || !text)
    return -1;

  for (char *p = text; *p; ++p) {
    if (*p == '\n') {
      continue;
    }
    io_oled_write_glyph(dev, font, *p);
  }
  return 0;
}

int io_oled_test(struct io_oled_device *dev) {

  io_oled_home(dev);

  for (uint8_t i = 0; i < 64; i++) {  // print half a line
    _io_oled_write(dev, 0xff);
  }

  io_oled_goto_line(dev, 1);          // new line

  for (uint8_t i = 0; i < 64; i++) {  // print half a line
    _io_oled_write(dev, 0xff);
  }

  io_oled_goto_line(dev, 2);          // new line
  io_oled_goto_column(dev, 0);        // new carrige return

  // fill every other column in a row
  for (uint8_t i = 0; i < 128; i++) {
    if (i % 2 == 0) {
      _io_oled_write(dev, 0xff);
    } else {
      _io_oled_write(dev, 0x00);
    }
  }

  io_oled_goto_line(dev, 3);          // new line
  io_oled_goto_column(dev, 0);        // new carrige return

  // fill a row with chess pattern
  for (uint8_t i = 0; i < 128; i++) {
    if (i % 2 == 0) {
      _io_oled_write(dev, 0b01010101);
    } else {
      _io_oled_write(dev, 0b10101010);
    }
  }

  io_oled_goto_line(dev, 4);   // new line
  io_oled_goto_column(dev, 0); // new carrige return
  io_oled_print_with_font(dev, &OLED_FONT_8x8, "Text 8x8");

  io_oled_goto_line(dev, 5);   // new line
  io_oled_goto_column(dev, 0); // new carrige return
  io_oled_print_with_font(dev, &OLED_FONT_5x7, "Text 5x7");

  io_oled_goto_line(dev, 6);   // new line
  io_oled_goto_column(dev, 0); // new carrige return
  io_oled_print_with_font(dev, &OLED_FONT_4x6, "Text 4x6");

  io_oled_goto_line(dev, 7);   // new line
  io_oled_goto_column(dev, 0); // new carrige return
  io_oled_print_with_font(dev, &OLED_FONT_8x8, "Test:Complete");

  return 0;
}

int io_oled_blink(struct io_oled_device *dev, uint8_t blinks) {

  io_oled_home(dev);

  for (uint8_t blink = 0; blink < 2 * blinks; blink++) {
    for (uint8_t row = 0; row < 8; row++) {
      io_oled_goto_line(dev, row); // new line
      io_oled_goto_column(dev, 0); // new carrige return
      for (uint8_t col = 0; col < 128; col++) {
        if (blink % 2 == 0) {
          _io_oled_write(dev, 0xFF);
        } else {
          _io_oled_write(dev, 0x00);
        }
      }
    }
    _delay_ms(500);
  }
  return 0;
}

int io_oled_write_data(struct io_oled_device *dev, uint8_t data) {
  return _io_oled_write(dev, data);
}

int io_oled_write_command(struct io_oled_device *dev, uint8_t command) {
  return _io_oled_cmd(dev, command);
}
