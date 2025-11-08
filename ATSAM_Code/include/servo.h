#ifndef INCLUDE_INCLUDE_SERVO_H_
#define INCLUDE_INCLUDE_SERVO_H_

#include "pwm.h"
#include <stdint.h>

struct Servo_device {
  struct PWM_device pwm;
  float max_dty;
  float mid_dty;
  float min_dty;
};

uint8_t servo_init(struct Servo_device *dev);
uint8_t servo_set_percentage(struct Servo_device *dev, uint8_t position);

#endif // INCLUDE_INCLUDE_SERVO_H_
