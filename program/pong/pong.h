// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------


#ifndef PONG_H
#define PONG_H

#include "component/windows_system/win.h"

void pong_init(win_context_t *ctx);
void pong_event_handler(win_context_t *ctx, wm_event_t *evt);
void pong_tick(void);

#endif 