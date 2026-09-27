// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------


#include "error.h"


#include "component/graphics_system/grap.h"
#include "component/graphics_system/colors.h"
static void sleep_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 4000; i++) {
        __asm__ __volatile__ ("outb %%al, $0x80" : : "a"(0));
    }
}

void error(const char *text) {
    uint32_t sw = fb_get_width();
    uint32_t sh = fb_get_height();

    
    fb_clear(0x000000FF);

    
    int logo_w = 1000;
    int logo_h = 600;
    int lx = (sw - logo_w) / 2;
    int ly = (sh - logo_h) / 2;

    draw_rect(lx - 4, ly - 4, logo_w + 8, logo_h + 8, COLOR_THINKPAD_RED);
    draw_rect(lx, ly, logo_w, logo_h, COLOR_BLACK);

    fb_draw_string(lx + 20, ly + 18, "Error! Kernel Panic", COLOR_BLACK, COLOR_RED);
    fb_draw_string(lx + 20, ly + 36, text, COLOR_THINKPAD_RED, COLOR_BLACK);
    fb_draw_string(lx + 20, ly + 54, "Atom Family System 32, Intel x86", COLOR_WHITE, COLOR_BLACK);

    
    int bar_w = 300;
    int bar_h = 8;
    int bx = (sw - bar_w) / 2;
    int by = ly + logo_h + 40;

    draw_rect(bx - 2, by - 2, bar_w + 4, bar_h + 4, 0x00333333);

    uint32_t step_delay =100;

    for (int step = 0; step <= 20; step++) {
        int current_w = (bar_w * step) / 100;
        draw_rect(bx, by, current_w, bar_h, COLOR_THINKPAD_RED);

        fb_swap_buffers();
        sleep_ms(step_delay);
    }
}