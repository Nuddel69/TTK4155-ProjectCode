/*
 * pid.h
 *
 * Created: 13.11.2025 08:26:21
 *  Author: wighu
 */

#ifndef PID_H_
#define PID_H_

#include "motor.h"
#include <stdint.h>

#define JOY_SENS 20

#define KP_DEFAULT 20
#define KI_DEFAULT 2
#define KD_DEFAULT 2

#define PID_MAX_OUT 20000
#define PID_MAX_WINDUP 1000

struct pid_controller {

  uint32_t Kp;
  uint32_t Ki;
  uint32_t Kd;

  int32_t integrator;
  
  int32_t prev_error;
  uint32_t last_time;

  int32_t MAX_out;
  int32_t MAX_windup;
};

uint8_t pid_init(struct pid_controller *PID, uint32_t Kp, uint32_t Ki,uint32_t Kd);
int32_t pid(int32_t inn, int32_t ref, struct pid_controller *PID);
uint8_t pid_reset(struct pid_controller *PID);
uint32_t pwm_dir_and_speed(struct motor_device *motor_dev,
                           struct pid_controller *pid_ctrl, int32_t pos_ref);



#endif /* PID_H_ */
