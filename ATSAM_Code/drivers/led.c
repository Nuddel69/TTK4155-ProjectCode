/*
 * led.c
 *
 * Created: 06.11.2025 10:15:56
 *  Author: wighu
 */ 

#include "sam.h"
#include "time.h"
#include <stdio.h>

#include "led.h"

uint8_t init_led(void){
	PIOB->PIO_OER  = (1 << 27);
}

	

uint8_t blink(void){
	time_spinFor(msecs(500));
	PIOB->PIO_SODR = (1 << 27);
    time_spinFor(msecs(500));
	PIOB->PIO_CODR = (1 << 27);
	  
} 