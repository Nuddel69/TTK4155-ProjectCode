/**
 * @file timer.h
 * @brief Timer driver (for ADC clock)
 * @defgroup TIMER Timer
 * @ingroup Hardware
 * @{
 */

#ifndef INCLUDE_INCLUDE_TIMER_H_
#define INCLUDE_INCLUDE_TIMER_H_

/**
 * @brief Initialize Timer1 for Fast PWM on OC1A.
 *
 * Configures PD5 as output, sets Fast PWM mode with ICR1 as top, and generates
 * ~50% duty cycle at approximately 983 kHz (with ICR1 = 4).
 *
 * @return 0 on success.
 */
int tim1_PWM_init(void);

/**
 * @brief Initialize Timer1 in CTC mode on OC1A.
 *
 * Configures PD5 as output and sets up CTC toggling with a compare value that
 * yields roughly 1–1.2 MHz output depending on clock and prescaler.
 *
 * @return 0 on success.
 */
int tim1_CTC_init(void);

#endif // INCLUDE_INCLUDE_TIMER_H_

/** @} */
