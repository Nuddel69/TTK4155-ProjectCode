/*
 * controller.h
 *
 * Created: 15.11.2025 13:53:09
 *  Author: wighu
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "can.h"
#include "io.h"

struct control_state {
  uint8_t game_over; // CAN_ID_GAMEOVER
};

uint8_t process_can_frame(struct control_state *crtl);
int8_t tx_joy_btn(struct io_joystick_device *joy_dev,
                  struct io_avr_device *avr_dev, struct can_device *can_dev);
int8_t tx_gamestart(struct can_device *can_dev);
int8_t tx_reset(struct can_device *can_dev);
int8_t tx_error(struct can_device *can_dev);

#endif /* CONTROLLER_H_ */
