#include <stdint.h>
#include <stdlib.h>

#include "encoder.h"
#include "motor.h"
#include "pid.h"
#include "time.h"

uint8_t pid_init(struct pid_controller *PID, uint32_t Kp, uint32_t Ki, uint32_t Kd) {

  PID->Kp = Kp;
  PID->Ki = Ki;
  PID->Kd = Kd;

  PID->integrator = 0;
  PID->prev_error = 0;
  PID->last_time = time_now();

  PID->MAX_out = PID_MAX_OUT;
  PID->MAX_windup = PID_MAX_WINDUP;

  return 0;
}

int32_t pid(int32_t inn, int32_t ref, struct pid_controller *PID) {

  //Find dt
  uint32_t now = time_now();
  uint32_t dt = msecs(now - PID->last_time);
  if (dt == 0){
	return 0;
  }
  PID->last_time = now;

  // Find current error
  int32_t error = (int32_t)ref - inn;
  
  //Deadband
  if (abs(error) < 50){
	  error = 0;
  }

  // Update current integral and anti windup
  if ((PID->integrator+error > -PID->MAX_windup) && (PID->integrator+error < PID->MAX_windup)){	
		PID->integrator += error * dt;
  }
/*
  if (PID->integrator > PID->MAX_windup) {
    PID->integrator = PID->MAX_windup;
  } else if (PID->integrator < (-PID->MAX_windup)) {
    PID->integrator = -PID->MAX_windup;
  }
 */

  // Calculate output
  int32_t outvalue = PID->Kp * error + 
					PID->Ki * PID->integrator +
                    PID->Kd * ((error - PID->prev_error) / dt);

  // Limit output
  uint32_t abs_out = abs(outvalue);
  if (abs_out > PID->MAX_out) {
    if (outvalue > 0) {
      outvalue = PID->MAX_out;
    } else if (outvalue < 0) {
      outvalue = -PID->MAX_out;
    }
  }
	
  //Remember error for next loop
  PID->prev_error = error;

  return outvalue;
}


uint8_t pid_reset(struct pid_controller *PID){
	
	PID->integrator = 0;          // Reset KI
	PID->prev_error = 0;          // Reset KD
	PID->last_time  = time_now(); // Stop from getting crazy spikes at next startup
}

uint32_t pwm_dir_and_speed(struct motor_device *motor_dev, struct pid_controller *pid_ctrl, int32_t pos_ref) {

  int32_t inn = (int32_t)TC2->TC_CHANNEL[0].TC_CV;
  int32_t motor_input = pid(inn, pos_ref, pid_ctrl);
  //printf("motor input:%d and current pos %d , go to ref %d \r\n",motor_input,inn, pos_ref);
  motor_dir_and_speed(motor_dev, motor_input);

  return 0;
}
