#ifndef INCLUDE_INCLUDE_SOLENOID_H_
#define INCLUDE_INCLUDE_SOLENOID_H_

#include <stdint.h>

#include "sam.h"

struct solenoid_device {
  Pio *port;
  uint8_t pin;
};

uint8_t solenoid_init(struct solenoid_device *dev);
uint8_t solenoid_pulse(struct solenoid_device *dev, uint8_t ms);
uint8_t solenoid_set_state(struct solenoid_device *dev, uint8_t state);

#endif // INCLUDE_INCLUDE_SOLENOID_H_
