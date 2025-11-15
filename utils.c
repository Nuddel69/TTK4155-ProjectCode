#include "utils.h"
#include "adc.h"
#include "io.h"
#include "uart.h"
#include <stdlib.h>
#include <util/delay.h>

int utils_test_address_decode(void) {

  volatile char *sram_adr = (char *)0x1800;
  volatile char *adc_adr = (char *)0x1500;

  (void)adc_adr[5];
  _delay_ms(5000);
  (void)sram_adr[5];
  _delay_ms(5000);

  return 0;
}

uint8_t utils_joystick_print_pos(struct io_joystick_position *pos) {
  struct ADC_meas data;

  ADC_read_all(&data);

  char raw_x_str[5] = "";
  char raw_y_str[5] = "";
  itoa(pos->x, raw_x_str, 10);
  itoa(pos->y, raw_y_str, 10);

  USART_SendString("Joystick X: ");
  USART_SendString(raw_x_str);
  USART_SendString("\t");

  USART_SendString("Joystick Y: ");
  USART_SendString(raw_y_str);
  USART_SendString("\n");

  return 0;
}

uint8_t utils_joystick_print_raw(struct io_joystick_device *dev) {
  struct ADC_meas data;

  ADC_read_all(&data);

  char raw_x_str[5] = "";
  char raw_y_str[5] = "";
  itoa(data.channel[dev->adc_channel_x], raw_x_str, 10);
  itoa(data.channel[dev->adc_channel_y], raw_y_str, 10);

  USART_SendString("Joystick X: ");
  USART_SendString(raw_x_str);
  USART_SendString("\t");

  USART_SendString("Joystick Y: ");
  USART_SendString(raw_y_str);
  USART_SendString("\n");

  return 0;
}

int16_t map(uint8_t x, uint8_t in_min, uint8_t in_max, int16_t out_min,
            int16_t out_max) {
  // do all math in 32-bit to avoid overflow
  int32_t num = (int32_t)(x - in_min) * (out_max - out_min);
  int32_t den = (int32_t)(in_max - in_min);
  int32_t result = num / den + out_min;
  return (int16_t)result;
}
