#include <stddef.h>
#include <util/delay.h>
#include <stdio.h>

#include "timer.h"
#include "uart.h"
#include "utils.h"
#include "xmem.h"
#include "adc.h"

#define FOSC 4915200
#define BAUD 9600

struct USART_config config = {BAUD, FOSC};

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
	
	printf("\n\r---Init Complete---\n\r");

	while (1) {
		// USART_ReceiveHandler(); // Required for USART-echo and command handling
		
		//Simple test of ADC		
		ADC_test();
	}
	return 0;
}