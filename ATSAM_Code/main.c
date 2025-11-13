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
#include "motor.h"
#include "pid.h"
// #include "../uart_and_printf/printf-stdarg.h"

#define DEBUG 0

#define baudrate 9600
#define F_CPU 84000000


//TEMP message for testing sending to node 1
CAN_MESSAGE dummy_msg ={{0x8},{8},{"HiWorld"}};
	
// struct PWM_device servo_pwm = ;
struct Servo_device servo = {{PIOB, 13, 1, 20000, 1500}, 900, 1500, 2100};
struct motor_device motor = {PIOC, 23, {PIOB, 12, 0, 20000, 12000}};
	
struct pid_controller motor_pid =  {KP_DEFAULT,KI_DEFAULT,KD_DEFAULT,0,0,0,PID_MAX_OUT,PID_MAX_WINDUP};
	
	
int main(void){

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
  
  motor_init(&motor);
  //PWM_init(&motor._enpw_dev);
  
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

  while (1) {
	  
	

  return 0;
}
