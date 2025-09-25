#include <avr/io.h>

//PWM, OC1A 50% dutycycle
void tim1_PWM_init(void) {
    // Set OC1A on PD5 as output
    DDRD |= (1 << PD5);

    // Fast PWM,
    //Set Compare Output Mode
    TCCR1A = (1 << COM1A1);
    //Set Clock to PWM fast mode, and prescaler 8(CS11)              
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11); 
    TCCR1A |= (1 << WGM11);

    uint16_t ICR1 = (FOSC/8*5000000)-1;   //frequency = fclk/(N*(1+ICR1))
    ICR1L = ICR1;
    ICR1H = (ICR1 >> 8);
    OCR1AL = ICR1/2; // 50% duty cycle
    OCR1AH = (ICR1/2 >> 8);

    // With 4915200 clk, N=8, freq =  4915200/(8*(1+)) = 5 MHz
}


// CTC timer, OC1A
void timer1_CTC_init(void) {
    // Set OC1A on PD5 as output
    DDRD |= (1 << PD5);

    // CTC mode, toggle OC1A on compare
    TCCR1A = (1 << COM1A0);
    TCCR1B = (1 << WGM12) | (1 << CS11);

    OCR1AL = (FOSC/2*8*5000000)-1;   // Compare value
    OCR1AH = ((FOSC/2*8*5000000)-1) >> 8;
    // With 4915200 Hz clk, N=8, freq =  4915200/(2*8*(1+)) = 5 MHz
}