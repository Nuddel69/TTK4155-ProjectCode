#include <stdint.h>

#include "motor.h"
#include "pwm.h"
#include "sam3x8e.h"
#include "pid.h"
#include "encoder.h"

uint8_t motor_init(struct motor_device *dev){

  // Initialise PWM pin
  //dev->_enpw_dev.port = dev->enpw;
  //dev->_enpw_dev.pin = dev->enqw_pin;
  //dev->_enpw_dev.channel = 0;
  //dev->_enpw_dev.period = dev->period;
  //dev->_enpw_dev.resting_dty = 0;
  PWM_init(&dev->_enpw_dev);
  PWM_set_dty(&dev->_enpw_dev, dev->_enpw_dev.resting_dty);

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
		dev->phdi->PIO_CODR |= 1 << dev->phdi_pin;
	}else{
		dev->phdi->PIO_SODR |= 1 << dev->phdi_pin;
	}
	
	//Set speed
	//PWM_set_dty(&dev->_enpw_dev, dev->_enpw_dev.resting_dty);
	PWM_set_dty(&dev->_enpw_dev, speed_abs);
	//PWM_set_dty(&dev->_enpw_dev, (dev->_enpw_dev.period) * (uint32_t)speed_abs);
	return 0;
	
}

uint8_t motor_left(struct motor_device *dev, uint8_t speed) {
  //PWM_set_dty(&dev->_enpw_dev, (dev->_enpw_dev.period / 100.0) * (int32_t)speed);
  dev->phdi->PIO_SODR |= 1 << dev->phdi_pin;
  return 0;
}

uint8_t motor_right(struct motor_device *dev, uint8_t speed) {
  //PWM_set_dty(&dev->_enpw_dev, (dev->_enpw_dev.period / 100.0) * (int32_t)speed);
  dev->phdi->PIO_CODR |= 1 << dev->phdi_pin;
  return 0;
}

uint8_t motor_dir(struct motor_device *dev, int8_t speed){
	if (speed>0){
		dev->phdi->PIO_SODR |= 1 << dev->phdi_pin;
	}else{
		dev->phdi->PIO_CODR |= 1 << dev->phdi_pin;
	}
	PWM_set_dty(&dev->_enpw_dev, (dev->_enpw_dev.period / 100.0) * (int32_t)speed);
}

uint8_t motor_stop(struct motor_device *dev) {
  PWM_set_dty(&dev->_enpw_dev, 0);
  return 0;
}

uint8_t motor_reset_pos(struct motor_device *dev){
	
	printf("Entering reset position \r\n");
	//Set inital conditions and start
	encoder_zero();
	int32_t pos = 0;
	int32_t last_pos = encoder_get_pos();
	uint32_t last_change = time_now();
	uint32_t start_time  = time_now();
	
	//Run until hit endstop
	while(1){
		printf("Still moving to endstop \r\n");
		motor_dir_and_speed(dev, 8000);

		pos = encoder_get_pos();

		// Encodervalue changed = still moving
		if (abs(pos-last_pos)>50) {
			last_pos = pos;
			last_change = time_now();
		}

		// If encoder not changed in 50ms
		if ((time_now() - last_change) > msecs(50)) {
			printf("Done, finding endpoint exiting while \r\n");
			break;
		}

		// encoder wont stop
		if (time_now() - start_time > msecs(500)) {
			printf("Waited 1 sec");
			motor_stop(dev);
			break;
		}
	}

	// Stop and reset encoder
	motor_stop(dev);
	encoder_zero();
	pos = encoder_get_pos();
	
	while (1) {
		printf("moving to center\r\n");
		motor_dir_and_speed(dev, -8000);
		// Check if past set point
		pos = encoder_get_pos();
		if (abs(pos) > 2500) {
			motor_dir_and_speed(dev, 0);
			printf("stopping at center\r\n");
			break;
		}
	}

	// Stop and make this new zero
	motor_stop(dev);
	encoder_zero();  

return 0; // success
}
