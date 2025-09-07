#ifndef XMEM_H
#define XMEM_H

#include <stdint.h>

#ifndef XMEM_3KB
#define XMEM_3KB 0   // Set to 1 if using 3KB memory instead of 2KB(0)
#endif

#if XMEM_3KB
//0x1400 offset and external memory size 0x0C00 if we decide to go for the 3KB solution
#define MEM_OFFSET 0x1400u
#define XMEM_SIZE 0x0C00u
#else
//0x1800 offset and external memory size 0x0800 if we decide to go for the 2KB solution
#define MEM_OFFSET 0x1800u
#define XMEM_SIZE 0x0800u
#endif

//Prototypes
/*!
 * \brief Enabels external memory, 2KB or 3KB based on XMEM_3KB.
 *
 */
void xmem_init(void);

/*!
 * \brief Tests entire external memory, prints a report that is 0 if memory is functioning properly.
 *
 */
void SRAM_test(void);

#endif XMEM_H