#ifndef INCLUDE_INCLUDE_IR_H_
#define INCLUDE_INCLUDE_IR_H_

#include <stdint.h>

// Prototypes
int adc_init(void);
uint16_t adc_read_once(void);
int attempt_score(uint8_t *score);
uint8_t simple_goal_detection(void)

#endif // INCLUDE_INCLUDE_IR_H
