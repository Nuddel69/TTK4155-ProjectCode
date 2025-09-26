#ifndef INCLUDE_INCLUDE_ADC_H_
#define INCLUDE_INCLUDE_ADC_H_

#include <stdint.h>

struct ADC_meas {
  uint8_t channel[4]; // Each channel hold ADC values of channel with
                      // corresponding index
};

void ADC_init(void);
void ADC_start_conv(void);
uint8_t ADC_read_ch(void);
void ADC_read_all(struct ADC_meas *output);
void ADC_test(void);

#endif // INCLUDE_INCLUDE_ADC_H_
