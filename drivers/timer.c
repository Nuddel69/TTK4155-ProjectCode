#include <avr/io.h>

//PWM, OC1A 50% dutycycle
void tim1_PWM_init(void) {
    // Set OC1A on PD5 as output
    DDRD |= (1 << PD5);

    // Fast PWM,
    //Set Compare Output Mode
    TCCR1A = (1 << COM1A1);
    //Set Clock to PWM fast mode, and prescaler 10(CS10)              
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10); 
    TCCR1A |= (1 << WGM11);

    ICR1L = 0x04; //frequency = fclk/(N*(1+ICR1)) 4 ~983khz
    ICR1H = ((0x04) >> 8);
    OCR1AL = ICR1/2; // 50% duty cycle
    OCR1AH = (ICR1/2 >> 8);
}

// CTC timer, OC1A
void tim1_CTC_init(void) {
    // Set OC1A on PD5 as output
    DDRD |= (1 << PD5);

    // CTC mode, toggle OC1A on compare
    TCCR1A = (1 << COM1A0);
    TCCR1B = (1 << WGM12) | (1 << CS10);

    OCR1AL = 0x01;   // Compare value
    OCR1AH = 0x01 >> 8;
    // With 4915200 Hz clk, N=8, value =  4915200/(2*8*(1+f_desierd)) = 1 MHz
    //1 = ~1.23Mhz
}