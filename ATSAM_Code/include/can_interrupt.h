/*
 * can_interrupt.h
 *
 * Author: Gustav O. Often and Eivind H. Jølsgard
 *
 * For use in TTK4155 Embedded and Industrial Computer Systems Design
 * NTNU - Norwegian University of Science and Technology
 *
 */ 

#ifndef CAN_INTERRUPT_H_
#define CAN_INTERRUPT_H_

#define CAN_RXQ_SIZE 16

#include "can_controller.h"

//Modified
void CAN0_Handler( void );


// Our functions
static inline int can_rxq_add(CAN_MESSAGE *msg);
int can_rxq_pull(CAN_MESSAGE *out);


#endif /* CAN_INTERRUPT_H_ */