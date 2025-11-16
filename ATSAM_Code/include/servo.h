#ifndef INCLUDE_INCLUDE_servo_H_
#define INCLUDE_INCLUDE_servo_H_

#include "pwm.h"
#include <stdint.h>

struct servo_device {
  struct PWM_device pwm;
  float max_dty;
  float mid_dty;
  float min_dty;
};

uint8_t servo_init(struct servo_device *dev);
uint8_t servo_set_percentage(struct servo_device *dev, uint8_t position);
uint8_t servo_set_range(struct servo_device *dev, int8_t position);

#endif // INCLUDE_INCLUDE_servo_H_
