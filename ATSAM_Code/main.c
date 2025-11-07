/*
 * Node2.c
 *
 * Created: 01.11.2025 08:30:28
 * Author : wighu
 */

#include <inttypes.h> // for printing time
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
// #include <util/delay.h>

#include "can_controller.h"
#include "led.h"
#include "sam.h"
#include "time.h"
#include "uart.h"
// #include "../uart_and_printf/printf-stdarg.h"

#define baudrate 9600
#define F_CPU 84000000

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

  // Can init
  if (can_init_def_tx_rx_mb() != 0) {
    printf("Failed to initialize CAN\r\n");
  }
  

  printf("-----Node2 Init complete------\r\n");
  
    //Manual IRQ test
  extern volatile uint32_t can0_irq_hits;  // define in can_interrupt.c
  NVIC_SetPendingIRQ(CAN0_IRQn);           // force an interrupt
  for (volatile int i=0;i<100000;i++){}    // brief spin
  printf("CAN0 hits=%lu\r\n", (unsigned long)can0_irq_hits);



  // TEST for checking if system is live, can be removed
  PIOB->PIO_OER = (1 << 27);
  
  printf("SR=0x%08lX IMR=0x%08lX  MB1:MMR=0x%08lX MSR=0x%08lX MCR=0x%08lX\r\n",
       (unsigned long)CAN0->CAN_SR,
       (unsigned long)CAN0->CAN_IMR,
       (unsigned long)CAN0->CAN_MB[1].CAN_MMR,
       (unsigned long)CAN0->CAN_MB[1].CAN_MSR,
       (unsigned long)CAN0->CAN_MB[1].CAN_MCR);

  uint64_t inittime = time_now();

  while (1) {

    time_spinFor(msecs(50));
    PIOB->PIO_SODR = (1 << 27);
    time_spinFor(msecs(50));
    PIOB->PIO_CODR = (1 << 27);

    printf("Current time: %.3f :3\r\n", totalSeconds(time_now() - inittime));
    // Check new RX mailbox 1
    if (can_receive(&msg, 1) == 0) {
      print_canmsg(&msg);
    }
    // Check new RX mailbox 2
    if (can_receive(&msg, 2) == 0) {
      print_canmsg(&msg);
    }
	
	// 1) Global CAN status (mailbox flags live here)
	uint32_t sr = CAN0->CAN_SR;
	if (sr & CAN_SR_MB1) { printf("[DBG] MB1 flag in CAN_SR\r\n"); }
	if (sr & CAN_SR_MB2) { printf("[DBG] MB2 flag in CAN_SR\r\n"); }

	// 2) Per-mailbox MRDY (set when RX mailbox has a frame)
	uint32_t msr1 = CAN0->CAN_MB[1].CAN_MSR;
	uint32_t msr2 = CAN0->CAN_MB[2].CAN_MSR;
	if (msr1 & CAN_MSR_MRDY) printf("[DBG] MB1.MRDY=1\r\n");
	if (msr2 & CAN_MSR_MRDY) printf("[DBG] MB2.MRDY=1\r\n");
	
  }
}

/* Initialize the SAM system */
// SystemInit();
