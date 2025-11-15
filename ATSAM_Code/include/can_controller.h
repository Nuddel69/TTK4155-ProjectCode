/*
 * can_controller.h
 *
 * Author: Gustav O. Often and Eivind H. J�lsgard
 *
 * For use in TTK4155 Embedded and Industrial Computer Systems Design
 * NTNU - Norwegian University of Science and Technology
 *
 */

#ifndef CAN_CONTROLLER_H_
#define CAN_CONTROLLER_H_

#include <stdint.h>

#define CAN_ID_ERROR     0x01
#define CAN_ID_GAMEOVER  0x02 //Sendt from NODE2, inform NODE1 that the game is over or lost a life aswell as its remaining lifes, and current score
#define CAN_ID_GAMESTART 0x03 //Tell game to start
#define CAN_ID_JOYPOS    0x04 //Joystick, and buttons
#define CAN_ID_RESET     0x05 //Inform game to move to standby state
#define CAN_ID_READY     0x06 //Not used
#define CAN_ID_SCORE     0x07 //Not used
#define CAN_ID_DEFAULT   0x08 

struct io_joystick_position {
	int x; // X value in % from the middle
	int y; // Y value in % from the middle
};


struct __attribute__((packed)) io_avr_buttons {
	union {
		uint8_t right;
		struct {
			uint8_t R1 : 1;
			uint8_t R2 : 1;
			uint8_t R3 : 1;
			uint8_t R4 : 1;
			uint8_t R5 : 1;
			uint8_t R6 : 1;
		};
	};
	union {
		uint8_t left;
		struct {
			uint8_t L1 : 1;
			uint8_t L2 : 1;
			uint8_t L3 : 1;
			uint8_t L4 : 1;
			uint8_t L5 : 1;
			uint8_t L6 : 1;
			uint8_t L7 : 1;
		};
	};
	union {
		uint8_t nav;
		struct {
			uint8_t NB : 1;
			uint8_t NR : 1;
			uint8_t ND : 1;
			uint8_t NL : 1;
			uint8_t NU : 1;
		};
	};
};

typedef struct can_message_t {
  uint16_t id;
  char data_length;
  char data[8];
} CAN_MESSAGE;

// Functions given by the course, some are modified
uint8_t can_init_def_tx_rx_mb(void);
uint8_t can_init(uint8_t num_tx_mb, uint8_t num_rx_mb);

uint8_t can_send(CAN_MESSAGE *can_msg, uint8_t mailbox_id);
uint8_t can_receive(CAN_MESSAGE *can_msg, uint8_t mailbox_id);

// Our own functions
uint8_t print_canmsg(const CAN_MESSAGE *msg);
void force_mailbox_layout_safe(void);

#endif /* CAN_CONTROLLER_H_ */
