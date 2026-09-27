// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------




#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

void interrupts_init(void);
int ps2_queue_has_data(void);
uint8_t ps2_queue_pop(void);

static inline void sti(void) { __asm__ __volatile__ ("sti"); }
static inline void cli(void) { __asm__ __volatile__ ("cli"); }
static inline void cpu_halt(void) { __asm__ __volatile__ ("hlt"); }

#endif