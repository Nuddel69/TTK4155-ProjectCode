/*
 * Node2.c
 *
 * Created: 01.11.2025 08:30:28
 * Author : wighu
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "can_controller.h"
#include "can_interrupt.h"
#include "encoder.h"
#include "game.h"
#include "ir.h"
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

struct io_joystick_position joy_pos = {0, 0};
struct io_avr_buttons btn;
uint8_t game_start = 0;

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
      // CAN_MESSAGE	msg = {0x02,0x8,0xFF};
      // can_send(&msg,0);
      game_start = 1;
      printf("STARTING NEW GAME\r\n");

      break;
    }
    case CAN_ID_JOYPOS: { // This ID is reserved for sending Joystick position
                          // and button state
      joy_pos.x = (int8_t)msg.data[0];
      joy_pos.y = (int8_t)msg.data[1];
      btn.right = (uint8_t)msg.data[2];
      btn.left = (uint8_t)msg.data[3];
      btn.nav = (uint8_t)msg.data[4];

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

struct servo_device servo = {{PIOB, 13, 1, 20000, 1500}, 2100, 1500, 900};
struct motor_device motor = {PIOC, 23, {PIOB, 12, 0, 20000, 000}};
struct solenoid_device solenoid = {PIOB, 25};
struct pid_controller motor_pid = {KP_DEFAULT, KI_DEFAULT,  KD_DEFAULT,    0, 0,
                                   0,          PID_MAX_OUT, PID_MAX_WINDUP};

// Set config
struct game_config game_conf = {&motor,   &motor_pid, &servo,     &solenoid,
                                &joy_pos, &btn,       &game_start};

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
  solenoid_init(&solenoid);
  motor_init(&motor);

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

  uint8_t coconut = 0;

  printf("-----Node2 Init complete------\r\n");

  while (1) {
    process_can_frame();

    // Reset encoder 0 position when pressing button
    if (btn.R4 && !button_r4_fired) {
      printf("Resetting encoder ref");
      encoder_zero();
      motor_stop(&motor);
      button_r4_fired = 1;
    }
    if (!btn.R4 && button_r4_fired) {
      button_r4_fired = 0;
    }

    uint16_t adc_value = adc_read_once();
    printf("ADC:%d\r\n", adc_value);

    basic_game(&game_conf);
  }
  return 0;
}
