#include <avr/io.h>
#include <util/delay.h>

#define ADC_BASE_ADR   ((volatile uint8_t*)0x1000) //Adress offset for ADC


typedef struct {
    uint8_t channel[4];// Each channel hold ADC values of channel with corresponding index
} ADC_meas;


void ADC_init(void) {

    //Set read and start conversion pins for ADC as outputs
    DDRB |= (1<<PB1)|(1<<PB2);
}

void ADC_start_conv(void){

    PORTB &= ~(1 << PB1);//Toggle WR LOW
    //Delay to ensure ADC conversion is done, consider checking BUSY pin insead.
    _delay_us(5);
    PORTB |= (1 << PB1);//Toggle WR HIGH
}

uint8_t read_single_ch(void){

    //Send WR
    PORTB &= ~(1 << PB2);//Toggle RD LOW
    //Delay, 1 us (with 1 MHz CLK) should be enough
    _delay_us(1);

    //Read datapins of the ADC
    volatile uint8_t data = PINA;

    PORTB |= (1 << PB2);//Toggle RD HIGH
    _delay_us(1);

    return data;

}


void ADC_read_all(ADC_meas *output) {

    ADC_start_conv();

    uint8_t adc_value[4];

    //Send a pulse then read for each channel
    for (uint8_t i = 0; i < 4; i++) {
        output->channel[i] = read_single_ch();
        }

}



/*
// Read all channels sequentially after BUSY goes high (3.6 us per ch)
void ADC_read(uint8_t *read_to_adr) {

    for (uint8_t i=0; i<3; i++) {
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





