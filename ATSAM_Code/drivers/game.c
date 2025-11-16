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



//CAN_MESSAGE	game_over_msg = {0x02,0x8,{1,2,3,4,5,6,7,8}};
						
/*

uint8_t legacy_game(struct motor_device *motor_dev,struct pid_controller *motor_pid){


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
*/


uint8_t basic_game(struct game_config *config){
	
	static uint8_t reset_complete = 0;
	static uint8_t button_r6_fired = 0;
	static uint8_t game_state = game_standby;
	//printf("Entering game");
	switch (game_state){
		
		case game_standby:
			printf("entering game standby\r\n");
			//Reset game
			if(!reset_complete){
				//motor_reset_pos(motor_dev);  // Implement this if we have time
				motor_stop(config->motor_dev);
				reset_complete = 1;
			}
			
			//Wait for startsignal from node 1
			if (config->game_start){
				 game_state = game_playing;
				config->game_start = 0;
				//pid_reset(motor_pid); //implement if we have time
			}
			break;
			
		case game_playing:
			//printf("entering game playing\r\n");
			//Give control to player
			pwm_dir_and_speed(config->motor_dev,config->motor_pid,(config->joy->x)*JOY_SENS);
			
			if (config->btn->R6 && !button_r6_fired) {
				solenoid_pulse(config->solenoid_dev, 40);
				button_r6_fired = 1;
			    }
			    if (!config->btn->R6 && button_r6_fired) {
				    button_r6_fired = 0;
			    }
			servo_set_range(config->servo_dev, config->joy->y);
			
				
				//End when goal is detected
				if(config->btn->L5){  //simple_goal_detection()){  //Added a dedicated button to lose as the IR on some of the motor boxes is broken
					printf("GOAL------------------------------------------------------\r\n");
					motor_stop(config->motor_dev);
					printf("SENDING GAME OVER----------------------------------------------");
					CAN_MESSAGE	game_over_msg = {0x02,0x8,{1,2,3,4,5,6,7,8}};
					can_send(&game_over_msg,0);
					*config->game_start = 0;
					game_state = game_standby;
					reset_complete = 0;
					time_spinFor(seconds(2));
				}
				break;
			
			
		default:
			printf("Undefined game state\r\n");
		
	}
	return 0;
}

/*
CAN_MESSAGE	game_over_msg = {game_over_msg.id=0x02,
	game_over_msg.data_length=0x8,
	game_over_msg.data[0]=1,
	game_over_msg.data[1]=2,
	game_over_msg.data[2]=3,
	game_over_msg.data[3]=4,
	game_over_msg.data[4]=5,
	game_over_msg.data[5]=6,
	game_over_msg.data[6]=7,
	game_over_msg.data[7]=8
};
*/