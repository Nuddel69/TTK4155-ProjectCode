#include <stdint.h>

#include "pwm.h"
#include "sam.h"

#define _MCK 84000000
#define _MCK_DIV 128

uint8_t PWM_init(struct PWM_device *dev) {
  // Enable the clock for PWM - Uses Master Clock (MCK) (PID36 from Table 9-1 in
  // datasheet)
  PMC->PMC_PCER1 |= PMC_PCER1_PID36;

  // Disable the PIO controller (GPIO) for the device pin, allowing the
  // peripheral A or B to take control of the pin function
  (dev->port)->PIO_PDR |= 1u << (dev->pin);

  // Select PWM as the peripheral controlling the pin
  (dev->port)->PIO_ABSR |=(1u << (dev->pin)); // Set peripheral function B for the device pin

  // Disable PWM write protect (WPKEY = PWM in ASCII)
  PWM->PWM_WPCR |= (PWM_WPCR_WPCMD(dev->channel) 
					|1u << (dev->channel + 2) /* Offset to account for cmd */ 
					|PWM_WPCR_WPKEY(0x50574D));

  // MCK prescaling - 84MHz/128
  PWM->PWM_CH_NUM[dev->channel].PWM_CMR |= PWM_CMR_CPRE_MCK_DIV_128;

  // CPRD = ((Period*MCK)/128), Period in us
  int cprd = ((dev->period) *0.000001 *_MCK) / _MCK_DIV;
  PWM->PWM_CH_NUM[dev->channel].PWM_CPRD |= PWM_CPRD_CPRD(cprd);

  // Set the initial dutycycle
  PWM_set_dty(dev, dev->resting_dty);

  // Enable PWM write protect
  PWM->PWM_WPCR |= (PWM_WPCR_WPCMD(dev->channel) | PWM_WPCR_WPRG1 |
                    PWM_WPCR_WPKEY(0x50574D));

  // Enable PWM
  PWM->PWM_ENA |= 1u << dev->channel;

  return 0;
}

uint8_t PWM_set_dty(struct PWM_device *dev, uint32_t dty) {
  // CDTY = ((Period-DTY)*MCK)/128, DTY in us
  int cdty = (((dty) * 0.000001) * _MCK) / _MCK_DIV;
  PWM->PWM_CH_NUM[dev->channel].PWM_CDTYUPD |= PWM_CDTY_CDTY(cdty);
}


