#include <avr/io.h>
#include <stdio.h>

#include "uart.h"
#include "xmem.h"

// Initialize external memory
void xmem_init(void) {
  // SpecialFunction IO Register
  // Enable external memory interface, datasheet ATmega162 pg18 & 26
  MCUCR |= (1 << SRE);

  // Release pins used for JTAG
  // The pins in range PC7-PC4 is reserved for JTAG, and XMBK to stop pins from
  // floating see pg32 datasheet
  // SFIOR =
  //     (SFIOR & ~((1 << XMM1) | (1 << XMM0))) | ((1 << XMM2) /* | (1 <<
  //     XMBK)*/);
  SFIOR |= (1 << XMM2);

  // Extended MCU Control Register pg30 datasheet
  // Setting SRL2-SRL0 as 1 gives the sectors lower sector= 0x1100-0xDFFF & high
  // sector =0xE000-=xFFFF Setting SRW01 & SRW00 as 1 gives waitstate 2 cycles
  // before writing and 1 cycle before sending new address, see pg 31
  // EMCUCR =
  //     (1 << SRW01) | (1 << SRW00) & ~(1 << SRL2) & ~(1 << SRL1)6(1 << SRL0);

  // EMCUCR = (1 << SRW11 | 1 << SRW10) & ~(SRL0) & ~(SRL1) & ~(SRL2);
  // EMCUCR = 0;

  // MCU Control Register pg30
  // Same as EMCUCR but for upper sector, set to wait 1 cycle for read & write,
  // see table at pg31
  // MCUCR = (MCUCR & ~(1 << SRW11)) | (1 << SRW10);
}

// Test taken from Lab-tasks
void SRAM_test(void) {
  volatile uint8_t *ext_ram = (uint8_t *)MEM_OFFSET; // Start adress for SRAM
  const uint16_t ext_ram_size = XMEM_SIZE;

  uint16_t write_errors = 0;
  uint16_t retrieval_errors = 0;

  printf("Starting SRAM test...\n\r");

  // rand() stores internal state; use it once to vary the seed each run
  uint16_t seed = (uint16_t)rand();

  // Write phase: write and verify immediately
  srand(seed);
  for (uint16_t i = 0; i < ext_ram_size; i++) {
    uint8_t value = (uint8_t)rand();
    ext_ram[i] = value;
    uint8_t retrieved = ext_ram[i];
    if (retrieved != value) {
      printf("Write phase error: ext_ram[%4u] = 0x%02X (should be 0x%02X)\n\r",
             i, (unsigned)retrieved, (unsigned)value);
      write_errors++;
    }
  }

  // Retrieval phase: ensure values didn't change after writes
  srand(seed); // reset PRNG to same sequence
  for (uint16_t i = 0; i < ext_ram_size; i++) {
    uint8_t value = (uint8_t)rand();
    uint8_t retrieved = ext_ram[i];
    if (retrieved != value) {
      printf(
          "Retrieval phase error: ext_ram[%4u] = 0x%02X (should be 0x%02X)\n\r",
          i, (unsigned)retrieved, (unsigned)value);
      retrieval_errors++;
    }
  }

  printf(
      "SRAM test completed with\n\r%4u errors in write phase and\n\r%4u errors "
      "in retrieval phase\n\r\n\r",
      write_errors, retrieval_errors);
}
