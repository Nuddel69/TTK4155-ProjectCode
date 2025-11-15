/*
 * game.h
 *
 * Created: 14.11.2025 10:11:03
 *  Author: wighu
 */ 


#ifndef GAME_H_
#define GAME_H_

#include <stdint.h>
#include "motor.h"
#include "pid.h"
#include "controller.h"

enum{
	game_standby,
	game_playing,
	game_ready,
	game_over,
	};

uint8_t game(struct motor_device *motor_dev,struct pid_controller *motor_pid, struct control_state *ctrl);

#endif /* GAME_H_ */