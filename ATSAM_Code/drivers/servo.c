#include <stdint.h>

#include "pwm.h"
#include "sam.h"
#include "servo.h"

#define _DTY_MIN 0x30F6 // 0.9 ms
#define _DTY_MID 0x2F6C // 1.5 ms
#define _DTY_MAX 0x2DE2 // 2.1 ms
#define _DTY_STEP

uint8_t servo_init(struct Servo_device *dev) {
  PWM_init(&dev->pwm);
  return 0;
}

static uint8_t _servo_max_dty(struct Servo_device *dev) {
  PWM_set_dty(&dev->pwm, dev->max_dty);
  return 0;
}

static uint8_t _servo_min_dty(struct Servo_device *dev) {
  PWM_set_dty(&dev->pwm, dev->min_dty);
  return 0;
}

uint8_t servo_set_percentage(struct Servo_device *dev, uint8_t position) {
  if (position >= 100) {
    _servo_max_dty(dev);
  }
  if (position <= 0) {
    _servo_min_dty(dev);
  }

  PWM_set_dty(&dev->pwm,
              dev->min_dty +
                  (position * ((dev->max_dty - dev->min_dty) / 100)));
  return 0;
}

uint8_t servo_set_range(struct Servo_device *dev, int8_t position) {

  // Clamp input
  if (position >= 127)
    position = 127;
  if (position <= -127)
    position = -127;

  uint16_t duty;

  if (position == 0) {
    duty = dev->mid_dty;
  } else if (position > 0) {
    // Scale: 0 -> mid_dty, 127 -> max_dty
    duty = dev->mid_dty +
           ((int32_t)position * (dev->max_dty - dev->mid_dty)) / 127;
  } else {
    // Scale: 0 -> mid_dty, -127 -> min_dty
    duty = dev->mid_dty +
           ((int32_t)position * (dev->mid_dty - dev->min_dty)) / 127;
  }

  PWM_set_dty(&dev->pwm, duty);
  return 0;
}
