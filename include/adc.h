#ifndef INCLUDE_INCLUDE_ADC_H_
#define INCLUDE_INCLUDE_ADC_H_

#include <stdint.h>

typedef struct {
    uint8_t channel[4];// Each channel hold ADC values of channel with corresponding index
} ADC_meas;

void ADC_init(void);
void ADC_start_conv(void);
uint8_t ADC_read_ch(void);
void ADC_read_all(ADC_meas *output);
void ADC_test(void);

#endif // INCLUDE_INCLUDE_ADC_H_
