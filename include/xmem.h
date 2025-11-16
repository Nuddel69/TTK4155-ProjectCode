/**
 * @file xmem.h
 * @brief External SRAM interface for ATmega microcontrollers.
 * @defgroup XMEM External Memory (SRAM)
 * @ingroup Hardware
 * @{
 */

#ifndef XMEM_H
#define XMEM_H

#include <stdint.h>

#ifndef XMEM_3KB
#define XMEM_3KB 0 /**< Set to 1 if using 3KB memory instead of 2KB */
#endif

#if XMEM_3KB
// 0x1400 offset and external memory size 0x0C00 if we decide to go for the 3KB
// solution
#define MEM_OFFSET 0x1400u /**< Base address for external SRAM */
#define XMEM_SIZE 0x0C00u  /**< Size of external SRAM: 3KB */
#else
// 0x1800 offset and external memory size 0x0800 if we decide to go for the 2KB
// solution
#define MEM_OFFSET 0x1800u /**< Base address for external SRAM */
#define XMEM_SIZE 0x0800u  /**< Size of external SRAM: 2KB */
#endif

/**
 * @brief Initializes external SRAM (2KB or 3KB depending on XMEM_3KB)
 * @return 0 on success, non-zero on error
 */
int xmem_init(void);

/**
 * @brief Tests external SRAM and prints a report
 * @return 0 if memory is OK
 * @return -1 if write phase failed
 * @return -2 if retrieval phase failed
 */
int SRAM_test(void);

#endif // XMEM_H

/** @} */
