#include <avr/io.h>
#include <util/delay.h>

#define ADC_BASE_ADR   ((volatile uint8_t*)0x1000) //Adress offset for ADC

uint8_t ADC_init(void) {
    //Set read and start conversion pins for ADC

    return ;
}


// Read all channels sequentially after BUSY goes high (3.6 us per ch)
void ADC_read(uint8_t *read_to_adr) {
    for (uint8_t i=0; i<3; i++) {
        //Select channel (sets PA1 and PA2 depending on channel, before pulsing RD)
        DDRA = 0xFF; // Set to outputs
        PINA = (1 << i);
        
        //Send WR
        PINB ^= (1 << PB1);//Toggle WR
        DDRA = 0x00; //Set to inputs  
        // Safe delay, 5 us per channel (with 5 MHz CLK) should be enough.
        _delay_us(5);

        PINB ^= (1 << PB2);//ToggleRD
        _delay_us(5);
        //Read channel on the ADC
        volatile uint8_t data = PINA
        read_to_adr[i] = data; 
    }
}






