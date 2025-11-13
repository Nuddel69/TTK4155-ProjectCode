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
#include "sam.h"
#include "sam3x8e.h"
#include "servo.h"
#include "solenoid.h"
#include "time.h"
#include "uart.h"

#define DEBUG 0

#define baudrate 9600
#define F_CPU 84000000

// TEMP message for testing sending to node 1
CAN_MESSAGE dummy_msg = {0x8, 8, {"HiWorld"}};

struct io_joystick_position joy_pos = {0, 0};
struct io_avr_buttons btn;

int process_can_frame() {

  CAN_MESSAGE msg;
  if (can_rxq_pull(&msg)) {

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
      printf("Buttons R=0x%02X L=0x%02X N=0x%02X, pos x:%d, y:%d\r\n",
             btn.right, btn.left, btn.nav, joy_pos.x, joy_pos.y);
      // update_control(joy_pos, btn);

      // update_control(joy_pos, btn); //TODO

      break;
    }
    case CAN_ID_SOLONOID: { // This ID is reserved for sending trigger signal
                            // for the solonoid

      // TODO: trigger solonoid

      break;
    }
    case CAN_ID_MOTORPOS: { // This ID is reserved for sending current motor
                            // position

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
}

// struct PWM_device servo_pwm = ;
struct Servo_device servo = {{PIOB, 13, 1, 20000, 1500}, 900, 1500, 2100};
struct motor_device motor = {PIOC, 23, {PIOB, 12, 0, 20000, 000}};
struct solenoid_device solenoid = {PIOB, 25};

struct pid_controller motor_pid = {KP_DEFAULT, KI_DEFAULT,  KD_DEFAULT,    0, 0,
                                   0,          PID_MAX_OUT, PID_MAX_WINDUP};

int main(void) {

  SystemInit();

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
  
  solenoid_init(&solenoid);

  motor_init(&motor);
  // PWM_init(&motor._enpw_dev);

  if (encoder_init() != 0) {
    // Print relevant register values
    encoder_debug_dump();
    printf("Failed to initialize CAN\r\n");
  }

  // Turn Watchdog off
  WDT->WDT_MR = WDT_MR_WDDIS;
  
  
  uint64_t inittime = time_now();
  
  //Reset Timer and PID at start
  motor_pid.last_time = inittime;
  TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_SWTRG;
  uint32_t counter = 0;

  uint8_t button_fired = 0;

  printf("-----Node2 Init complete------\r\n");


  while (1) {

    process_can_frame();
    pwm_dir_and_speed(&motor, &motor_pid, (joy_pos.x-27)*50);
    int32_t inn = (int32_t)TC2->TC_CHANNEL[0].TC_CV;
	printf("Current X ref:%d and Xpos:%d \r\n",joy_pos.x,inn);

    if (btn.R6 && !button_fired) {
      solenoid_pulse(&solenoid, 40);
      button_fired = 1;
    }
    if (!btn.R6 && button_fired) {
      button_fired = 0;
    }
	
	/*
	if (btn.R5) {
		servo_set_percentage(&servo, 100);
	}
	if (btn.R4) {
		servo_set_percentage(&servo, 0);
	}
	
	servo_set_percentage(&servo, 50);
	*/
	/*
	time_spinFor(84000000);
	servo_set_percentage(&servo, 0);
	time_spinFor(84000000);
	servo_set_percentage(&servo, 100);
	*/	
	
	//uint16_t IR_val = adc_read_once();
	//printf("IR;%d \r\n",IR_val);
    // time_spinFor(100);
  }
  return 0;
}
