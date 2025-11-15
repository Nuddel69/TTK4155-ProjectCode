#ifndef INCLUDE_INCLUDE_MOTOR_H_
#define INCLUDE_INCLUDE_MOTOR_H_

#include "pwm.h"
#include "sam.h"
#include <stdint.h>

#define MAX_MOTOR_SPEED 20000 //MAX PWM dutycycle in us
#define MIN_MOTOR_SPEED 0  //Min PWM dutycycle in us


struct motor_device {
  Pio *phdi;
  uint8_t phdi_pin;
  struct PWM_device _enpw_dev;
};

uint8_t motor_init(struct motor_device *dev);

uint8_t motor_left(struct motor_device *dev, uint8_t speed);
uint8_t motor_right(struct motor_device *dev, uint8_t speed);
uint8_t motor_stop(struct motor_device *dev);

uint8_t motor_dir_and_speed(struct motor_device *dev,int32_t speed);
uint8_t motor_reset_pos(struct motor_device *dev);

#endif // INCLUDE_INCLUDE_MOTOR_H_
