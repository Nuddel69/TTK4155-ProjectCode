/*
 * Node2.c
 *
 * Created: 01.11.2025 08:30:28
 * Author : wighu
 */


#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
// #include <util/delay.h>

#include "can_controller.h"
#include "can_interrupt.h"
#include "encoder.h"
#include "ir.h"
#include "led.h"
#include "motor.h"
#include "pid.h"
#include "pwm.h"
#include "sam.h"
#include "sam3x8e.h"
#include "servo.h"
#include "solenoid.h"
#include "time.h"
#include "uart.h"
#include "game.h"

#define DEBUG 0

#define baudrate 9600
#define F_CPU 84000000

struct io_joystick_position joy_pos = {0, 0};
struct io_avr_buttons btn;
uint8_t game_start = 0;

int process_can_frame() {
  CAN_MESSAGE msg;
  if (can_rxq_pull(&msg)) {
		//printf("Rx msg with id %d\r\n",msg.id);
    switch (msg.id) {		

    case CAN_ID_ERROR: { // This ID is reserved for errors, BOTH node1 and node2

      // TODO: handle error / stop game

      break;
    }
    case CAN_ID_GAMEOVER: { // This ID is reserved for gameover message from
                            // node2

      break;
    }
    case CAN_ID_GAMESTART: { // This ID is reserved for starting a new game from
                             // node1
		//CAN_MESSAGE	msg = {0x02,0x8,0xFF};				 
		//can_send(&msg,0);
		game_start=1;
		printf("STARTING NEW GAME\r\n");

      // TODO: start game

      break;
    }
    case CAN_ID_JOYPOS: { // This ID is reserved for sending Joystick position
                          // and button state
      joy_pos.x = (int8_t)msg.data[0];
      joy_pos.y = (int8_t)msg.data[1];
      btn.right = (uint8_t)msg.data[2];
      btn.left = (uint8_t)msg.data[3];
      btn.nav = (uint8_t)msg.data[4];
	  
      // printf("%c[2J",27);
      //printf("Buttons R=0x%02X L=0x%02X N=0x%02X, pos x:%d, y:%d\r\n",
      //      btn.right, btn.left, btn.nav, joy_pos.x, joy_pos.y);
      // update_control(joy_pos, btn);

      // update_control(joy_pos, btn); //TODO

      break;
    }
    case CAN_ID_SCORE: { // This ID is reserved for sending gamescore

      break;
    }
    case CAN_ID_DEFAULT: { // This ID is for anything else

      break;
    }
    default: {
    }
    }
  }
  return 0;
}

// struct PWM_device servo_pwm = ;
struct servo_device servo = {{PIOB, 13, 1, 20000, 1500}, 2100, 1500, 900};
//struct Servo_device motor_srv = {{PIOB, 12, 0, 20000, 1500}, 2100, 1500, 900};
struct motor_device motor = {PIOC, 23, {PIOB, 12, 0, 20000, 000}};
struct solenoid_device solenoid = {PIOB, 25};

struct pid_controller motor_pid = {KP_DEFAULT, KI_DEFAULT,  KD_DEFAULT,    0, 0,
                                   0,          PID_MAX_OUT, PID_MAX_WINDUP};
//Set config
struct game_config game_conf = {&motor,
	&motor_pid,
	&servo,
	&solenoid,
	&joy_pos,
	&btn,
	&game_start
};

int main(void) {

  SystemInit();
  
    // Turn Watchdog off
  WDT->WDT_MR = WDT_MR_WDDIS;

  int status = 0;
  CAN_MESSAGE msg;

  // UART init
  if (uart_init(F_CPU, baudrate) != 0) {
    printf("Failed to initialize UART\r\n");
  }

  if (adc_init() != 0) {
    printf("Failed to initialize ADC\r\n");
  }

  // Can init
  if (can_init_def_tx_rx_mb() != 0) {
    printf("Failed to initialize CAN\r\n");
  }

  servo_init(&servo);
  //servo_init(&motor_srv);

  solenoid_init(&solenoid);

  motor_init(&motor);
  // PWM_init(&motor._enpw_dev);

  if (encoder_init() != 0) {
    // Print relevant register values
    encoder_debug_dump();
    printf("Failed to initialize CAN\r\n");
  }


	

  uint64_t inittime = time_now();

  // Reset Timer and PID at start
  motor_pid.last_time = inittime;
  TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_SWTRG;
  
  uint32_t counter = 0;
  uint8_t button_r6_fired = 0;
  uint8_t button_r4_fired = 0;

  printf("-----Node2 Init complete------\r\n");

  //uint8_t coconut = 0;
  
  while (1) {
	process_can_frame();
	//Reset encoder 0 position when pressing button 
	if (btn.R4 && !button_r4_fired) {
		printf("Resetting encoder ref");
		encoder_zero();
		motor_stop(&motor);
		button_r4_fired = 1;
    }
    if (!btn.R4 && button_r4_fired) {
		button_r4_fired = 0;
    }
	
	basic_game(&game_conf);

  }
  return 0;
}
