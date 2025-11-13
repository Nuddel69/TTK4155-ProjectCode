/*
 * adc.c
 *
 * Created: 09.11.2025 11:27:27
 *  Author: wighu
 */

#include "ir.h"
#include "sam.h"

// Init a single ADC channel in freerun mode
int adc_init(void) {

  // Enable power to ADC
  PMC->PMC_PCER1 |= PMC_PCER1_PID37; // ID_ADC=37 PCER1 bit 5

  // Disable writeprotect (p.1353 datasheet)
  ADC->ADC_WPMR =
      ADC_WPMR_WPKEY_PASSWD; // ADC in ASCII shifted 8 (0x414443)<<8);

  // Enable chosen channel(AD7 ch=0)
  ADC->ADC_CHER = ADC_CHER_CH0;

  // SET MODE (P.1333 datasheet)
  // 12-bit, freerun, 1 MHz
  //  ADCClock = 84MHz / ((41+1)*2) = 1.0 MHz
  ADC->ADC_MR = ADC_MR_FREERUN | ADC_MR_PRESCAL(41) | ADC_MR_TRACKTIM(10) |
                ADC_MR_TRANSFER(2) | ADC_MR_STARTUP_SUT64;

  // Enable writeprotect (p.1353 datasheet), do we need a WP?
  // ADC->ADC_WPMR =  ADC_WPMR_WPKEY_PASSWD|1; //ADC in ASCII shifted 8 +1
  // (0x414443)<<8)|1;

  // Start conversion, Since it is in freerun we only need to start it once. and
  // read (uint16_t)(ADC->ADC_LCDR & 0xFFF) whenever flag is set
  ADC->ADC_CR = ADC_CR_START;

  return 0;
}

static inline uint16_t adc_read(void) {
  while ((ADC->ADC_ISR & ADC_ISR_DRDY) == 0) {
    // Wait until read is ready
  }
  // Mask because 12bit ADC
  return (uint16_t)(ADC->ADC_CDR[0] & 0x0FFF);
}

// TEMP function to see if we actually read the ADC
uint16_t adc_read_once(void) {
  while ((ADC->ADC_ISR & ADC_ISR_DRDY) == 0) {
    // Wait until read is ready
  }
  // Mask because 12bit ADC
  return (uint16_t)(ADC->ADC_CDR[0] & 0x0FFF);
}

int attempt_score(uint8_t *score) {

  // STATE
  static uint16_t baseline = 0; // EMA baseline
  static uint8_t init_done = 0; // baseline initialized?
  static uint8_t blocked = 0;   // "IR blocked" state
  static uint8_t count = 0;     // debounce counter

  uint16_t meas = adc_read();
  uint16_t exit_thr, enter_thr = 0;

  if (!init_done) {
    uint32_t sum = meas;
    // Avg 32 samples
    for (int i = 0; i < 31; i++)
      sum += adc_read();
    baseline = (uint16_t)(sum >> 5);
    init_done = 1;
  } else {
    // EMA: baseline = baseline+(meas-baseline)/16 (>> is same as 1/16)
    // Only alter baseline if not blocked
    if (!blocked) {
      baseline += (int16_t)(meas - baseline) >> 4;
    }
  }

  // Debouncing
  // Enter "blocked" when meas < (baseline - 150)
  if (baseline > 150) {
    uint16_t enter_thr = baseline - 150;
  } else {
    uint16_t enter_thr = 0;
  }
  // Exit  "blocked" when meas >= (baseline - 100)
  if (baseline > 100) {
    uint16_t exit_thr = baseline - 100;
  } else {
    uint16_t exit_thr = 0;
  }

  if (meas < exit_thr) {
    int blocked = 1;
  } else if (meas > enter_thr) {
    int blocked = 0;
  }

  if (blocked) {
    if (count < 8) {
      count++;
    }
  } else {
    if (count > 0)
      count--;
  }

  if (!blocked && count >= 8) {
    blocked = 1;
    (*score)++;
  }
  if (blocked && count == 0) {
    blocked = 0;
  }
  return 0;
}
