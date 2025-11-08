#ifndef INCLUDE_INCLUDE_PWM_H_
#define INCLUDE_INCLUDE_PWM_H_

#include "sam.h"
#include <stdint.h>

struct PWM_device {
  Pio *port;
  uint8_t pin;
  uint8_t channel;

  uint32_t period;   //* Period in ms
  float resting_dty; //* The initial duty_cycle of the PWM device
};

uint8_t PWM_init(struct PWM_device *dev);
uint8_t PWM_set_dty(struct PWM_device *dev, float dty);
uint8_t PWM_max_dty(struct PWM_device *dev);
uint8_t PWM_min_dty(struct PWM_device *dev);
#endif // INCLUDE_INCLUDE_PWM_H_
