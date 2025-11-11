/*
 * encoder.c
 *
 * Created: 09.11.2025 22:55:58
 *  Author: wighu
 */ 

#include "sam.h"
#include "encoder.h"
#include <stdio.h>
#include "time.h"

int8_t encoder_init(void) {
	
	//All registers listed on page 629++(GPIO) 879(TC) and in the datasheet.
	
	//Disable write protect for PMC 
	PMC->PMC_WPMR  = PMC_WPMR_WPKEY_PASSWD;
	
	//Enable clock
	//TC2 clock (ID 1(33)), in PCER1
	PMC->PMC_PCER1 |= PMC_PCER1_PID33;
	
	//DEBUG Check no block from WP
	if(PMC->PMC_WPSR & 1){
		printf("Write protect couldn't be removed, writing to PMC clock for encoder failed");
		return 1;
	}

	//Disable PIO WP, for some reason it wont let us write to BMR, this is just a test
	PIOC->PIO_WPMR = (PIO_WPMR_WPKEY(0x50494F)); //PW is PIO is Ascii
	PIOC->PIO_WPMR &= ~PIO_WPMR_WPEN;

	//DEBUG Check before writing to pins
	uint32_t absr_val = PIOC->PIO_ABSR;
	printf("Pre writing ABSR=%d  \r\n",absr_val);
	
	//Set PC25 for TIOA6 and PC26 for TIOB6
	// Disable PIO control on pins
	PIOC->PIO_PDR = ENCODER_A_PIN | ENCODER_B_PIN;
	// Select Peripheral B for encoder pin A and B
	PIOC->PIO_ABSR |= ENCODER_A_PIN | ENCODER_B_PIN;
	// Disable pull-up
	PIOC->PIO_PUDR = ENCODER_A_PIN | ENCODER_B_PIN;
	
	//DEBUG Check before writing to pins
	absr_val = PIOC->PIO_ABSR;
	printf("Check after writing ABSR=%d  \r\n",absr_val);
	
	//Check if writeprotect triggered
	if ((PIOC->PIO_WPSR > 0)&1){
		printf("Write protect couldn't be removed, writing to PIO for encoder failed");
		return 1;
	}	

	// CH0 clock must be enabled to access WPMR, disable WP, key "TIM" in ASCII
	TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN;
	TC2->TC_WPMR = TC_WPMR_WPKEY_PASSWD;
	//TC2->TC_WPMR &= ~TC_WPMR_WPEN;
	
	//Checking BMR before setting value
	uint32_t bmr = TC2->TC_BMR;
	printf("Pre writing BMR=%d  \r\n",bmr);
	
	//Configure TC2 for Quadrature decode on CH0 , see page 901++
	// Enable quadrature decoder, position mode, count on both A and B edges,
	// enable small filter (1-63),
	//If we need to reverse polarity add, |TC_BMR_INVA|TC_BMR_INVB
	bmr = TC_BMR_QDEN				// enable QDEC
		|TC_BMR_POSEN               // position measurement on ch0
		|TC_BMR_EDGPHA              // count on both A and B edges, for higher resolution
		|TC_BMR_MAXFILT(3);         // simple input filter
	
	printf("writing BMR=%d  \r\n",bmr);
	TC2->TC_BMR = bmr;
	time_spinFor(5);
	bmr = TC2->TC_BMR;
	printf("checking BMR=%d  \r\n",bmr);
	

	// Channel 0, Capture mode, clock source XC0
	// TCCLKS = XC0, 0b101 (5)
	TC2->TC_CHANNEL[0].TC_CMR = TC_CMR_TCCLKS_XC0;

	//Start counting (enable clock)
	TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
	
	//DEBUG
	uint32_t psr  = PIOC->PIO_PSR;
    uint32_t absr = PIOC->PIO_ABSR;
    uint32_t bmr1  = TC2->TC_BMR;
    uint32_t cmr0 = TC2->TC_CHANNEL[0].TC_CMR;
    printf("PSR=%d ABSR=%d BMR=%d CMR0=%d\r\n",
           psr,absr,bmr1,cmr0);
	
	return 0;
}


static inline void encoder_zero(void) {
	// Reset zero ref
	TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_SWTRG;
}

static inline int32_t encoder_get_pos(void) {
	// 32-bit count value from CH0
	return (int32_t)TC2->TC_CHANNEL[0].TC_CV;
}

//TEMP TESTING FUNCTIONS
void encoder_test_print(void) {

	int32_t pos = encoder_get_pos();
	printf("%d\r\n", pos);
	
}

void encoder_debug_dump(void) {
    uint32_t psr = PIOC->PIO_PSR;
	uint32_t absr = PIOC->PIO_ABSR;
	uint32_t bmr = TC2->TC_BMR;
	uint32_t cmr0 = TC2->TC_CHANNEL[0].TC_CMR;
	printf("PSR=%d ABSR=%d BMR=%d CMR0=%d\r\n",
       psr,absr,bmr,cmr0);

}