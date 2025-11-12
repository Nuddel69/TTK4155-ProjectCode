#include <stdint.h>
#include "motor.h"


PID_init(struct pid_controller *PID,uint32_t Kp,uint32_t Ki,uint32_t Kd){
	
	PID->Kp = Kp;
	PID->Ki = Ki;
	PID->Kd = Kd;

	PID->integrator = 0;
	PID->prev_in = 0;
	PID->last_time = time_now(); 
  
	PID->MAX_out = MAX_MOTOR_SPEED;
	PID->MAX_windup = MAX_WINDUP;
	
}


uint8_t PID(uint8_t inn, uint8_t ref,struct pid_controller *PID){


	uint32_t dt = time_now() - PID->last_time;   

	//Find current error
	uint16_t pos=(int16_t)encoder_get_pos();
	int16_t error = (int16_t)ref - pos;

	//Update current integral
	PID->integrator += error*dt;

	// Anti windup for integral
	if (PID->integrator > PID->MAX_WINDUP){
		PID->integrator = PID->MAX_WINDUP;
	}else if(PID->integrator < -PID->MAX_WINDUP){
		PID->integrator = -PID->MAX_WINDUP;
	}

	//Calculate output
	int32_t outvalue = PID->Kp*error+PID->Ki*PID->integrator-PID->Kd*(pos-PID->prev_in)/dt;

	//Limit output
	if abs(outvalue > PID->MAX_out){
		if (outvalue > 0){
			outvalue = PID->MAX_out;
		}else if(outvalue<0){
			outvalue = -PID->MAX_out;
		}

	PID->prev_in = inn;
	
	return outvalue;
}


// uint8_t Kalman(state){}