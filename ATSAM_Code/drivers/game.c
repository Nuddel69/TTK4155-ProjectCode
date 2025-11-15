/*
 * game.c
 *
 * Created: 14.11.2025 14:55:55
 *  Author: wighu
 */ 

#include "game.h"
#include "motor.h"
#include "pid.h"
#include "can_controller.h"
#include "time.h"
#include "can_controller.h"
#include "controller.h"

uint8_t game(struct motor_device *motor_dev,struct pid_controller *motor_pid, struct control_state *ctrl){


	static uint8_t just_died = 0;
	static uint8_t reset_complete =1;
	static uint32_t score = 0;
	static uint32_t game_starttime = 0;
	static uint8_t game_state = game_standby;
	
	switch(game_state){
	
		case game_standby:{
			
			if (!reset_complete){
				//Reset motorposition and stop motor
				motor_reset_pos(&motor_dev);
				motor_stop(&motor_dev);
				reset_complete = 1;
			}
			
			//Reset game 
			ctrl->lifes = 3;
			ctrl->score = 0;
			
			//Startgame if button pressed
			if (ctrl->game_start){
				ctrl->game_start = 0;
				reset_complete = 0;
				pid_reset(&motor_pid);
				game_starttime = time_now();
				game_state = game_playing;
				break;
			}
			
			break;
		}
		case game_playing:{
			
			//Start motor
			pwm_dir_and_speed(&motor_dev, &motor_pid, (-ctrl->joy_x) * JOY_SENS);
			
			if (ctrl->lifes <=0){
				game_state = game_over;
				break;
			}
			
			//Check goal
			if (check_goal(ctrl->lifes)){
				just_died=1;
				game_state = game_ready;
				break;
			}
			
			break;
		}
		case game_ready:{
			
			
			if (ctrl->lifes <=0){
				game_state = game_over;
				break;
			}
			
			//Add accumulated score
			if (just_died){
				score += seconds(time_now()-game_starttime);
				just_died = 0;
			}
			
			//Send game over message to node 1
			CAN_MESSAGE gameover_msg = {0x2, 8, {{ctrl->score},{ctrl->lifes},{0},{0},{0},{0},{0},{0}}};
			can_send(&gameover_msg, 1);
			
			//Start next life
			if (ctrl->game_start){
				ctrl->game_start = 0;
				pid_reset(&motor_pid);
				game_starttime = time_now();
				game_state = game_playing;
			} 
			break;
		}
		
		case game_over:{
			
			//Stop game
			motor_stop(&motor_dev);
			score += seconds(time_now()-game_starttime);
			
			//Send game over message to node 1
			CAN_MESSAGE gameover_msg = {0x2, 8, {{ctrl->score},{ctrl->lifes},{0},{0},{0},{0},{0},{0}}};	
			can_send(&gameover_msg, 1);
			
			//Leave on standby
			if (ctrl->game_reset){
				ctrl->game_reset = 0;
				game_state = game_standby;
				break;
			}
			
			//Immediate restart
			if (ctrl->game_start){
				
				ctrl->game_start = 0;
				//Reset system for next game
				ctrl->lifes = 3;
				ctrl->score = 0;
				game_starttime = time_now();
				pid_reset(&motor_pid);
				
				game_state = game_playing;
				break;
			}
			break;
		}
		default:{
			printf("Undefined gamestate");
			return -1;
		}
	}
	return 0;
}