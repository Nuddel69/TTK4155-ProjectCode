#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#include "adc.h"

#define ADC_BASE_ADR ((volatile uint8_t *)0x1000) // Adress offset for ADC

void ADC_init(void) {

  // XMEM controls the ADC and is enabled elsewhere (xmem_init())
  // Do NOT touch DDR/PORTA (PORTA is the external data bus and is handeled
  // trough writing to and reading to xmem) No need to set up PD6/PD7; XMEM
  // controls WR/RD automatically when we read or write to memory allocted to
  // the ADC
}

void ADC_start_conv(void) {

  // Write to any adress on ADC to trigger WR
  *ADC_BASE_ADR = 0x00;
  // Delay to ensure ADC conversion is done, checking BUSY pin insead.
  while (PORTD & PD4) {
  }
}

uint8_t ADC_read_ch(void) {

  // Read from any adress in ADC allocated memory adressarea to
  return *ADC_BASE_ADR;
}

void ADC_read_all(ADC_meas *output) {

  ADC_start_conv();

  uint8_t adc_value[4];

  // Send a pulse then read for each channel
  for (uint8_t i = 0; i < 4; i++) {
    output->channel[i] = ADC_read_ch();
    _delay_us(5);
  }
}

void ADC_test(void) {

  ADC_meas data;

  ADC_read_all(&data);

  printf("--ADC Test--\n\r");
  printf("CH0=%3d  CH1=%3d  CH2=%3d  CH3=%3d\r\n", data.channel[0] - 128,
         data.channel[1] - 128, data.channel[2] - 128, data.channel[3] - 128);

  _delay_ms(100);
}

/*
// Read all channels sequentially after BUSY goes high (3.6 us per ch)
void ADC_read(uint8_t *read_to_adr) {

    for (uint8_t i=0; i<3; i++) {
        //Select channel (sets PA1 and PA2 depending on channel, before pulsing
RD) DDRA = 0xFF; // Set to outputs PINA =  i;

        //Send WR
        PINB ^= (1 << PB1);//Toggle WR
        DDRA = 0x00; //Set to inputs
        // Safe delay, 5 us per channel (with 1 MHz CLK) should be enough.
        _delay_us(5);

        PINB ^= (1 << PB2);//Toggle RD
        _delay_us(5);
        //Read channel on the ADC
        volatile uint8_t data = PINA
        read_to_adr[i] = data;
    }
    DDRA = 0xFF;
}


uint8_t ADC_read_ch(uint8_t ch){

    //Select channel (sets PA1 and PA2 depending on channel, before pulsing RD)
        DDRA = 0xFF; // Set to outputs
        PINA =  i;

        //Send WR
        PINB ^= (1 << PB1);//Toggle WR
        DDRA = 0x00; //Set to inputs
        // Safe delay, 5 us per channel (with 1 MHz CLK) should be enough.
        _delay_us(5);

        PINB ^= (1 << PB2);//Toggle RD
        _delay_us(5);
        //Read channel on the ADC
        volatile uint8_t data = PINA
        read_to_adr[i] = data;

}
*/
