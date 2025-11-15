/*
 * controller.h
 *
 * Created: 15.11.2025 13:22:31
 *  Author: wighu
 */ 


#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "can_controller.h"
#include "can_interrupt.h"

struct control_state{
	int8_t  joy_x;
	int8_t  joy_y;
	struct io_avr_buttons btn;

	uint8_t game_start;   // CAN_ID_GAMESTART
	uint8_t game_reset;   // CAN_ID_STANDBY
	uint8_t game_over;    // CAN_ID_GAMEOVER
	uint8_t score;        // Current game score
	uint8_t lifes;        // Number of lives left in game
	uint8_t error_flag;   // CAN_ID_ERROR
};

uint8_t process_can_frame(struct control_state *crtl);

#endif /* CONTROLLER_H_ */