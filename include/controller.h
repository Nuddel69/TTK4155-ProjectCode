/**
 * @file controller.h
 * @brief Handles CAN communication for game control, joystick, and buttons.
 * @defgroup CONTROLLER Game Controller / CAN
 * @ingroup Communication
 * @{
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "can.h"
#include "io.h"

/**
 * @brief Stores current control state
 */
struct control_state {
  uint8_t game_over; /**< 1 if game is over (CAN_ID_GAMEOVER) */
};

/**
 * @brief Processes an incoming CAN frame
 * @param ctrl Pointer to control state struct
 * @return 0 on success
 */
uint8_t process_can_frame(struct control_state *ctrl);

/**
 * @brief Transmits joystick positions and button states via CAN
 * @param joy_dev Pointer to joystick device
 * @param avr_dev Pointer to I/O board device
 * @param can_dev Pointer to CAN device
 * @return 0 on success, negative on error
 */
int8_t tx_joy_btn(struct io_joystick_device *joy_dev,
                  struct io_avr_device *avr_dev, struct can_device *can_dev);

/**
 * @brief Sends a game start message via CAN
 * @param can_dev Pointer to CAN device
 * @return 0 on success
 */
int8_t tx_gamestart(struct can_device *can_dev);

/**
 * @brief Sends a reset message via CAN
 * @param can_dev Pointer to CAN device
 * @return 0 on success
 */
int8_t tx_reset(struct can_device *can_dev);

/**
 * @brief Sends an error message via CAN
 * @param can_dev Pointer to CAN device
 * @return 0 on success, negative on error
 */
int8_t tx_error(struct can_device *can_dev);

#endif /* CONTROLLER_H_ */

/** @} */
