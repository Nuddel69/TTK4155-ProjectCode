#include <stdint.h>

#include "sam.h"
#include "solenoid.h"
#include "time.h"

uint8_t solenoid_init(struct solenoid_device *dev) {
  // Enable PIO controll of the solenoid-pin
  dev->port->PIO_PER |= 1 << dev->pin;

  // Set the solenoid pin as output
  dev->port->PIO_OER |= 1 << dev->pin;

  // Clear the solenoid pin
  dev->port->PIO_SODR |= 1 << dev->pin;

  return 0;
}
uint8_t solenoid_pulse(struct solenoid_device *dev, uint8_t ms) {
  dev->port->PIO_CODR |= 1 << dev->pin;
  time_spinFor(msecs(ms));
  dev->port->PIO_SODR |= 1 << dev->pin;
  return 0;
}

uint8_t solenoid_set_state(struct solenoid_device *dev, uint8_t state) {
  if (state) {
    dev->port->PIO_CODR |= 1 << dev->pin;
    if (!(dev->port->PIO_ODSR & (1 << dev->pin))) {
      return 0;
    }
    return -1;
  }
  dev->port->PIO_SODR |= 1 << dev->pin;
  if ((dev->port->PIO_ODSR & (1 << dev->pin))) {
    return 0;
  }
  return -1;
}
