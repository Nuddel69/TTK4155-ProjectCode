#include <stddef.h>
#include <util/delay.h>

#include "can.h"
#include "io.h"
#include "log.h"
#include "spi.h"
#include "timer.h"
#include "uart.h"
#include "utils.h"
#include "xmem.h"

#define BAUD 9600

LOG_MODULE_DEFINE("main")

// Device Configs
struct USART_config config = {BAUD, F_CPU};
struct io_joystick_device joy = {0, 1, 0, 0};
struct io_avr_device avr = {SSB3};
struct can_device can = {SSE2};

// Data Containers
struct io_joystick_position pos;
struct CAN_frame can_msg = {0xFACB, 0x08, "CAN Dumb", 0, 0};
struct io_avr_buttons btn;
struct CAN_frame dummy_msg;



int tx_joy_btn(struct io_joystick_device *joy_dev,struct io_avr_device *avr_dev, struct can_device *can_dev){
	
	//Read ADC
	struct io_joystick_position joy_pos;
	io_joystick_read_position(joy_dev, &joy_pos);
	
	struct io_avr_buttons btn;
	io_avr_buttons_read(avr_dev,&btn);
	
	//Frame Data
	struct CAN_frame msg = {CAN_ID_JOYPOS,0x08,{joy_pos.x,joy_pos.y,btn.right,btn.left,btn.nav,0,0,0},0, 0};
	
	//Transmit data
	can_write(&can, msg);
	
	
	return 0;
}

int tx_gamestart(struct can_device *can_dev){
	
	//Frame Data
	struct CAN_frame msg = {CAN_ID_GAMESTART,0x08,{0,0,0,0,0,0},1, 0};
	
	//Transmit data
	can_write(&can, msg);
	return 0;
}

int tx_error(struct can_device *can_dev){
	
	//Frame Data
	struct CAN_frame msg = {CAN_ID_ERROR,0x08,{0,0,0,0,0,0},1, 0};
	
	//Transmit data
	can_write(&can, msg);
	return 0;
}

int process_can_frame(struct CAN_frame *can_frame){
	
	switch (can_frame->id){
		
		case CAN_ID_ERROR:{   //This ID is reserved for errors, BOTH node1 and node2
			
			STATUS_ASSERT(1);
			break;
		}
		case CAN_ID_GAMEOVER:{//This ID is reserved for gameover message from node2
			
			break;
		}
		case CAN_ID_GAMESTART:{//This ID is reserved for starting a new game from node1
			
			break;
		}
		case CAN_ID_JOYPOS:{  //This ID is reserved for sending Joystick position and button state
			
			break;
		}
		case CAN_ID_SOLONOID:{//This ID is reserved for sending trigger signal for the solonoid
			
			break;
		}
		case CAN_ID_MOTORPOS:{//This ID is reserved for sending current motor position
			
			break;
		}
		case CAN_ID_SCORE:{   //This ID is reserved for sending gamescore
			
			break;
		}
		case CAN_ID_DEFAULT:{ //This ID is for anything else
			break;
		}
		default:{
			
		}
	}
	
}


int main() {
  int status = 0;

  status = USART_init(&config);
  STATUS_ASSERT(status);

  status = xmem_init();
  STATUS_ASSERT(status)

  status = SRAM_test();
  STATUS_ASSERT(status)

  status = tim1_CTC_init();
  STATUS_ASSERT(status)

  status = io_avr_init(&avr);
  STATUS_ASSERT(status)

  status = can_init(&can);
  STATUS_ASSERT(status)

  LOG_INF("---Init complete---")

  while (1) {
    tx_joy_btn(&joy,&avr,&can);
    // USART_SendString("Hello World!\n");
	_delay_ms(100);
	
	while (can_rxq_pull(&dummy_msg)) {
		process_can_frame(&dummy_msg);
	}
  }
}
