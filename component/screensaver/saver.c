
// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------

#include "saver.h"
#include "component/graphics_system/grap.h"
#include "component/graphics_system/colors.h"
#include "logo.h"

static void sleep_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 4000; i++) {
        __asm__ __volatile__ ("outb %%al, $0x80" : : "a"(0));
    }
}

static void draw_fullscreen_bitmap(const uint8_t *bitmap) {
    for (int y = 0; y < BOOT_SCREEN_HEIGHT; y++) {
        for (int x = 0; x < BOOT_SCREEN_WIDTH; x++) {
            int byte_index = (y * BOOT_SCREEN_WIDTH + x) / 8;
            int bit_index = 7 - ((y * BOOT_SCREEN_WIDTH + x) % 8);

            if ((bitmap[byte_index] >> bit_index) & 1) {
                fb_put_pixel(x, y, 0x00000000);
            } else {
                fb_put_pixel(x, y, 0x00FFFFFF);
            }
        }
    }
}

void saver_show_boot(uint32_t duration_ms) {
    (void)duration_ms;

    draw_fullscreen_bitmap(boot_screen_bitmap);

    int bar_w = 280;
    int bar_h = 20;
    int bx = 830;  
    int by = 420;  

    draw_rect(bx - 3, by - 3, bar_w + 6, bar_h + 6, 0x00000000);
    draw_rect(bx, by, bar_w, bar_h, 0x00FFFFFF);

    fb_swap_buffers();

    for (int step = 0; step <= 100; step += 2) {
        int current_w = (bar_w * step) / 100;
        
        draw_rect(bx, by, current_w, bar_h, 0x00000000);

        fb_swap_buffers();
        sleep_ms(25);
    }

    sleep_ms(300);
}