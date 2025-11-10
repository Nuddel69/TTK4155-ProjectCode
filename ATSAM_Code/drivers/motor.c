#include <stdint.h>

#include "motor.h"
#include "pwm.h"
#include "sam3x8e.h"

uint8_t motor_init(struct motor_device *dev) {

  // Initialise PWM pin
  dev->_enpw_dev.port = dev->enpw;
  dev->_enpw_dev.pin = dev->enqw_pin;
  dev->_enpw_dev.channel = 0;
  dev->_enpw_dev.period = dev->period;
  dev->_enpw_dev.resting_dty = 0;
  PWM_init(&dev->_enpw_dev);

  // Initialise direction GPIO pin
  dev->phdi->PIO_PER |= 1 << dev->phdi_pin;
  dev->phdi->PIO_OER |= 1 << dev->phdi_pin;

  return 0;
}

uint8_t motor_left(struct motor_device *dev, uint8_t speed) {
  PWM_set_dty(&dev->_enpw_dev, (dev->period / 100.0) * (float)speed);
  dev->phdi->PIO_SODR |= 1 << dev->phdi_pin;
  return 0;
}

uint8_t motor_right(struct motor_device *dev, uint8_t speed) {
  PWM_set_dty(&dev->_enpw_dev, (dev->period / 100.0) * (float)speed);
  dev->phdi->PIO_CODR |= 1 << dev->phdi_pin;
  return 0;
}

uint8_t motor_stop(struct motor_device *dev) {
  PWM_set_dty(&dev->_enpw_dev, 0);
  return 0;
}
