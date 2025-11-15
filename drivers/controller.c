/*
 * controller.c
 *
 * Created: 15.11.2025 13:52:49
 *  Author: wighu
 */

#include "controller.h"
#include "can.h"
#include "io.h"

uint8_t process_can_frame(struct control_state *ctrl) {

  struct CAN_frame msg;
  if (can_rxq_pull(&msg)) {

    switch (msg.id) {

    case CAN_ID_ERROR: { // This ID is reserved for errors, BOTH node1 and node2

      // TODO: handle error / stop game

      break;
    }
    case CAN_ID_GAMEOVER: { // This ID is reserved for gameover message from
      // node2
      ctrl->lifes = msg.data[0];
      ctrl->score = msg.data[1];

      if (ctrl->lifes <= 0) {
        ctrl->game_over = 1;
      }
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

int8_t tx_joy_btn(struct io_joystick_device *joy_dev,
                  struct io_avr_device *avr_dev, struct can_device *can_dev) {

  // Read ADC
  struct io_joystick_position joy_pos;
  io_joystick_read_position(joy_dev, &joy_pos);

  struct io_avr_buttons btn;
  io_avr_buttons_read(avr_dev, &btn);

  // Frame Data
  struct CAN_frame msg = {
      CAN_ID_JOYPOS,
      0x08,
      {joy_pos.x, joy_pos.y, btn.right, btn.left, btn.nav, 0, 0, 0},
      0,
      0};

  // Transmit data
  can_write(can_dev, msg);

  return 0;
}

int8_t tx_gamestart(struct can_device *can_dev) {

	// Frame Data
	struct CAN_frame msg = {CAN_ID_GAMESTART, 0x01, {1}, 1, 0};

  // Transmit data
  can_write(can_dev, msg);
  return 0;
}

int8_t tx_reset(struct can_device *can_dev) {

	// Frame Data
	struct CAN_frame msg = {CAN_ID_RESET, 0x01, {0}, 1, 0};

  // Transmit data
  can_write(can_dev, msg);
  return 0;
}

int8_t tx_error(struct can_device *can_dev) {

	// Frame Data
	struct CAN_frame msg = {CAN_ID_ERROR, 0x08, {0}, 1, 0};

  // Transmit data
  can_write(can_dev, msg);
  return 0;
}
