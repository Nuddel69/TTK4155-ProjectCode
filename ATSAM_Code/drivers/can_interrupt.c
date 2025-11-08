/*
 * interrupt.c
 *
 * Author: Gustav O. Often and Eivind H. J�lsgard
 *
 * For use in TTK4155 Embedded and Industrial Computer Systems Design
 * NTNU - Norwegian University of Science and Technology
 *
 */ 

#include "can_interrupt.h"

#include <stdio.h>
#include "sam.h"

//#include "../uart_and_printf/printf-stdarg.h"

#include "can_controller.h"

#define DEBUG_INTERRUPT 0

static volatile uint8_t rxq_head = 0, rxq_tail = 0;
CAN_MESSAGE rxq[CAN_RXQ_SIZE];

/**
 * \brief CAN0 Interrupt handler for RX, TX and bus error interrupts
 *
 * \param void
 *
 * \retval 
 */
void CAN0_Handler( void )
{
	
	if(DEBUG_INTERRUPT)printf("CAN0 interrupt\n\r");
	uint32_t can_sr = CAN0->CAN_SR; 
	
	//RX interrupt
	if(can_sr & (CAN_SR_MB1 | CAN_SR_MB2) )//Only mailbox 1 and 2 specified for receiving
	{
		CAN_MESSAGE message;
		if(can_sr & CAN_SR_MB1)  //Mailbox 1 event
		{
			can_receive(&message, 1);
			can_rxq_add(&message);

		}
		else if(can_sr & CAN_SR_MB2) //Mailbox 2 event
		
		{
			can_receive(&message, 2);
			can_rxq_add(&message);
		}
		else
		{
			printf("CAN0 message arrived in non-used mailbox\n\r");
		}

		if(DEBUG_INTERRUPT)printf("message id: %d\n\r", message.id);
		if(DEBUG_INTERRUPT)printf("message data length: %d\n\r", message.data_length);
		for (int i = 0; i < message.data_length; i++)
		{
			if(DEBUG_INTERRUPT)printf("%d ", message.data[i]);
		}
		if(DEBUG_INTERRUPT)printf("\n\r");
	}

	
	if(can_sr & CAN_SR_MB0)
	{
		if(DEBUG_INTERRUPT) printf("CAN0 MB0 ready to send \n\r");
		
	//Disable interrupt
		CAN0->CAN_IDR = CAN_IER_MB0;

	}

	if(can_sr & CAN_SR_ERRP)
	{
		if(DEBUG_INTERRUPT)printf("CAN0 ERRP error\n\r");

	}
	if(can_sr & CAN_SR_TOVF)
	{
		if(DEBUG_INTERRUPT)printf("CAN0 timer overflow\n\r");

	}
	
	NVIC_ClearPendingIRQ(ID_CAN0);
	//sei();*/
}


//---------OUR FUNCTIONS
 // Add to RX Buffer
static inline int can_rxq_add(CAN_MESSAGE *msg) {
	
	uint8_t next = (uint8_t)((rxq_head + 1) & (CAN_RXQ_SIZE - 1));
	if (next == rxq_tail){ 
		return 0;
	}	 
	rxq[rxq_head] = *msg;
	rxq_head = next;
	return 1;
}

// Pull from RX buffer
int can_rxq_pull(CAN_MESSAGE *out){
	if (rxq_tail == rxq_head){
		return 0;
	}
	*out = rxq[rxq_tail];
	rxq_tail = (uint8_t)((rxq_tail + 1) & (CAN_RXQ_SIZE - 1));
	return 1;
}
