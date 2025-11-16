/**
 * @file adc.h
 * @brief ADC memory-mapped interface
 * @defgroup ADC ADC
 * @ingroup Hardware
 * @{
 */

#ifndef INCLUDE_INCLUDE_ADC_H_
#define INCLUDE_INCLUDE_ADC_H_

#include <stdint.h>

/**
 * @brief Container for ADC measurement results.
 *
 * Holds one 8-bit sample for each of the four ADC channels.
 * Channels correspond to physical ADC inputs 0–3.
 */
struct ADC_meas {
  uint8_t channel[4]; /**< ADC reading for each channel index (0–3). */
};

/**
 * @brief Initialize the external memory–mapped ADC interface.
 *
 * This prepares communication with the external ADC connected on the XMEM bus.
 * All low-level XMEM configuration must already be handled elsewhere
 * (e.g., via xmem_init()).
 */
void ADC_init(void);

/**
 * @brief Start a conversion on the external ADC and wait for completion.
 *
 * Triggers a write strobe through the ADC's memory interface and waits for
 * the BUSY flag (PD4) to clear.
 */
void ADC_start_conv(void);

/**
 * @brief Read the current ADC channel sample.
 *
 * Reads a single byte from the ADC's memory-mapped interface.
 *
 * @return The 8-bit ADC result of the currently selected channel.
 */
uint8_t ADC_read_ch(void);

/**
 * @brief Perform a full four-channel ADC conversion and return all results.
 *
 * Starts a conversion, then reads each channel sequentially with a small delay
 * to satisfy ADC timing requirements.
 *
 * @param[out] output Pointer to a structure that will receive channel data.
 */
void ADC_read_all(struct ADC_meas *output);

/**
 * @brief Internal ADC test routine. (Currently disabled in implementation.)
 *
 * Prints ADC values for channels 0–3 via UART.
 */
void ADC_test(void);

#endif // INCLUDE_INCLUDE_ADC_H_

/** @} */
