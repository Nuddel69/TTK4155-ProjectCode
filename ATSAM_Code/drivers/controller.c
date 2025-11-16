/*
 * controller.c
 *
 * Created: 15.11.2025 13:22:16
 *  Author: wighu
 */

/*
#include <stdio.h>

#include "can_controller.h"
#include "can_interrupt.h"
#include "controller.h"

uint8_t process_can_frame(struct control_state *ctrl) {

  CAN_MESSAGE msg;
  if (can_rxq_pull(&msg)) {

	  switch (msg.id) {

		  case CAN_ID_ERROR:  // This ID is reserved for errors, BOTH node1 and node2

			  // TODO: handle error / stop game

			  return 0;
		  
		  case CAN_ID_GAMEOVER:  // This ID is reserved for gameover message from
			  // node2
				break;
		  
		  case CAN_ID_GAMESTART:  // This ID is reserved for starting a new game from
			  // node1
				ctrl->game_start = 1;
				time_spinFor(84000000);
				  //msg.id = CAN_ID_GAMEOVER;
				  //msg.data_length = 0x8;
				  //msg.data = {'HiWorld'};
				CAN_MESSAGE d_msg = {0x3, 8, {"HiWorld"}};
			      can_send(&d_msg,1);
			  
			  return 0;
		  
		  case CAN_ID_JOYPOS:  // This ID is reserved for sending Joystick position
			  // and button state
			  ctrl->joy_x = (int8_t)msg.data[0];
			  ctrl->joy_y = (int8_t)msg.data[1];
			  ctrl->btn.right = (uint8_t)msg.data[2];
			  ctrl->btn.left = (uint8_t)msg.data[3];
			  ctrl->btn.nav = (uint8_t)msg.data[4];

			  // printf("%c[2J",27);
			  printf("Buttons R=0x%02X L=0x%02X N=0x%02X, pos x:%d, y:%d\r\n",
			  ctrl->btn.right, ctrl->btn.left, ctrl->btn.nav, ctrl->joy_x,
			  ctrl->joy_y);

			  return 0;
		 
		  case CAN_ID_RESET:  // This ID is reserved for RESET command,from node1 to
			  // node2, indicating that the game should return to
			  // standby

			  ctrl->game_reset = 1;

			  return 0;
		  
		  case CAN_ID_READY:  // This is not used

			  return 0;
		  
		  case CAN_ID_SCORE:  // This is not used

			  return 0;
		  
		  case CAN_ID_DEFAULT:  // This ID is for anything else

			  return 0;

		  
	  }
  }
  return 0;
  }
*/