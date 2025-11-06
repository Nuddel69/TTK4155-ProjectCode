/*
 * Node2.c
 *
 * Created: 01.11.2025 08:30:28
 * Author : wighu
 */ 


#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
//#include <util/delay.h>


#include "sam.h"
#include "can_controller.h"
#include "uart.h"
#include "led.h"
#include "time.h"
//#include "../uart_and_printf/printf-stdarg.h"   

#define baudrate 9600
#define F_CPU 84000000

int main(void){
	
	SystemInit();
	//Turn Watchdog off
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	int status = 0;
	CAN_MESSAGE msg;
	
	// UART init
	if (uart_init (F_CPU, baudrate) != 0) {
		printf("Failed to initialize UART\r\n");
		}
	
	// Can init
	if (can_init_def_tx_rx_mb() != 0) {
		printf("Failed to initialize CAN\r\n");
		}
	
	printf("-----Node2 Init complete------\r\n");
	
	//TEST for checking if system is live, can be removed
	PIOB->PIO_OER  = (1 << 27);
	
	while (1){ 
		
		time_spinFor(msecs(500));
		PIOB->PIO_SODR = (1 << 27);
		time_spinFor(msecs(500));
		PIOB->PIO_CODR = (1 << 27);
		
		
		printf("Hello World\r\n");
		printf("Test\r\n");
		// Check new RX mailbox 1
		if (can_receive(&msg, 1) == 0) {
			print_canmsg(&msg);
		}
		// Check new RX mailbox 2
		if (can_receive(&msg, 2) == 0) {
			print_canmsg(&msg);
		}
	}
}

    /* Initialize the SAM system */
    //SystemInit();
