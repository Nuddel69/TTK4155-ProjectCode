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

uint8_t motor_dir_and_speed(struct motor_device *dev,int32_t speed){
	
	// Find speed regardless of direction
	uint32_t speed_abs = abs(speed);
	
	// To slow to be worth moving
	if (speed_abs < MIN_MOTOR_SPEED){	
		speed_abs = 0;
	}
	// Hit Speedlimit
	if(speed_abs > MAX_MOTOR_SPEED){
		speed_abs = MAX_MOTOR_SPEED;
	}
	
	// Set direction
	if (speed>0){
		dev->phdi->PIO_SODR |= 1 << dev->phdi_pin;
	}else{
		dev->phdi->PIO_CODR |= 1 << dev->phdi_pin;
	}
	
	//Set speed
	PWM_set_dty(&dev->_enpw_dev, (dev->period) * (uint32_t)speed_abs);
	return 0;
	
}

uint8_t motor_left(struct motor_device *dev, uint8_t speed) {
  PWM_set_dty(&dev->_enpw_dev, (dev->period / 100.0) * (int32_t)speed);
  dev->phdi->PIO_SODR |= 1 << dev->phdi_pin;
  return 0;
}

uint8_t motor_right(struct motor_device *dev, uint8_t speed) {
  PWM_set_dty(&dev->_enpw_dev, (dev->period / 100.0) * (int32_t)speed);
  dev->phdi->PIO_CODR |= 1 << dev->phdi_pin;
  return 0;
}

uint8_t motor_dir(struct motor_device *dev, int8_t speed){
	if (speed>0){
		dev->phdi->PIO_SODR |= 1 << dev->phdi_pin;
	}else{
		dev->phdi->PIO_CODR |= 1 << dev->phdi_pin;
	}
	PWM_set_dty(&dev->_enpw_dev, (dev->period / 100.0) * (int32_t)speed);
}

uint8_t motor_stop(struct motor_device *dev) {
  PWM_set_dty(&dev->_enpw_dev, 0);
  return 0;
}
