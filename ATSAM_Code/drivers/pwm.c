#include "sam.h"
#include <stdint.h>

uint8_t PWM_init(void) {
  // Enable the clock for PWM - Uses Master Clock (MCK) (PID36 from Table 9-1 in
  // datasheet)
  PMC->PMC_PCER1 |= PMC_PCER1_PID36;

  // Disable the PIO controller (GPIO) for PB13, allowing the peripheral A or B
  // to take control of the pin function
  PIOB->PIO_PDR |= PIO_PB13B_PWMH1;

  // Select PWM as the peripheral controlling the pin
  PIOB->PIO_ABSR |= (PIO_PB13B_PWMH1); // Set peripheral function B for PB13

  // MCK prescaling - 84MHz/128
  PWM->PWM_CH_NUM[1].PWM_CMR |= PWM_CMR_CPRE_MCK_DIV_128;

  // CPRD = (Period*MCK)/128, Period in ms
  PWM->PWM_CH_NUM[1].PWM_CPRD |= PWM_CPRD_CPRD(0x3345);

  // CDTY = (DTY*MCK)/128, DTY in ms
  // PWM->PWM_CH_NUM[1].PWM_CDTY |= PWM_CDTY_CDTY(0x3D8);
  PWM->PWM_CH_NUM[1].PWM_CDTY |= PWM_CDTY_CDTY(0x2F6C);

  // Enable PWM
  PWM->PWM_ENA |= PWM_ENA_CHID1;
}
