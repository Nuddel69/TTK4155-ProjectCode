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
#include "servo.h"
#include "led.h"
#include "sam.h"
#include "time.h"
#include "uart.h"
#include "adc.h"
#include "encoder.h"
// #include "../uart_and_printf/printf-stdarg.h"

#define DEBUG 0

#define baudrate 9600
#define F_CPU 84000000


//TEMP message for testing sending to node 1
CAN_MESSAGE dummy_msg ={{0x8},{8},{"HiWorld"}};
	
// struct PWM_device servo_pwm = ;
struct Servo_device servo = {{PIOB, 13, 1, 20000, 1500}, 900, 1500, 2100};
	
	
int main(void) {

  SystemInit();

  int status = 0;
  CAN_MESSAGE msg;

  // UART init
  if (uart_init(F_CPU, baudrate) != 0) {
    printf("Failed to initialize UART\r\n");
  }
  
  if (adc_init()!=0){
	printf("Failed to initialize ADC\r\n");
  }

  // Can init
  if (can_init_def_tx_rx_mb() != 0) {
    printf("Failed to initialize CAN\r\n");
  }
  
  servo_init(&servo);
  
  if (encoder_init()!=0){
	//Print relevant register values
	encoder_debug_dump();
	printf("Failed to initialize CAN\r\n");
  }
  
  // Turn Watchdog off
  WDT->WDT_MR = WDT_MR_WDDIS;

  printf("-----Node2 Init complete------\r\n");
  
  uint64_t inittime = time_now();
  uint32_t counter = 0;
  //NVIC_DisableIRQ(ID_CAN0);

  while (1) {
		//Get message from buffer
		//if (can_rxq_pull(&msg)){
		//	print_canmsg(&msg);
		//}
		
		//process_can_frame();
		servo_set_percentage(&servo, 100);
		time_spinFor(msecs(1000));
		servo_set_percentage(&servo, 0);
		time_spinFor(msecs(1000));
		//if (can_send(&dummy_msg, 0) == 0) {
		//	printf("[TX] STD id=0x%03X dlc=%d\r\n", dummy_msg.id, dummy_msg.data_length);
		//	} else {
			//printf("[TX] mailbox busy\r\n");
		//}
		
		//uint16_t volts_12bit = adc_read_once();
		//uint16_t volts=(volts_12bit*3.3)/4096; //(ADC_value*Ref_max_voltage)/(12bit_max)
		
		//printf("Voltage ADC: %u\r\n",volts_12bit);
		
		int32_t pos = (int32_t)TC2->TC_CHANNEL[0].TC_CV;
		printf("%d\r\n",pos);
		}

    //time_spinFor(msecs(500));
	//printf("Still waiting\r\n");
    //PIOB->PIO_SODR = (1 << 27);
    //time_spinFor(msecs(50));
    //PIOB->PIO_CODR = (1 << 27);

    //printf("Current time: %.3f :3\r\n", totalSeconds(time_now() - inittime));
    // Check RX mailbox 1
    //if (can_receive(&msg, 1) == 0) {
	  //printf("Current time: %.3f :3\r\n", totalSeconds(time_now() - inittime));
      //print_canmsg(&msg);
    //}
    // Check RX mailbox 2
    //if (can_receive(&msg, 2) == 0) {
	  //printf("Current time: %.3f :3\r\n", totalSeconds(time_now() - inittime));
      //print_canmsg(&msg);

  //}
  return 0;
}
