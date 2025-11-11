/*
 * encoder.h
 *
 * Created: 10.11.2025 11:34:04
 *  Author: wighu
 */ 


#ifndef ENCODER_H_
#define ENCODER_H_

#define ENCODER_A_PIN     (1 << 25)   // PC25 TIOA6
#define ENCODER_B_PIN     (1 << 26)   // PC26 TIOB6

int8_t encoder_init(void);
static inline void encoder_zero(void);
static inline int32_t encoder_get_pos(void);

//TEMP TESTING FUNCTION
void encoder_test_print(void);
void encoder_debug_dump(void);

#endif /* ENCODER_H_ */