// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------


#include "print.h"

static int cursor_x = 0;
static int cursor_y = 0;

void print_clear(void) {
    volatile uint16_t *screen_buffer = (volatile uint16_t*)0xB8000;
    for (int i = 0; i < 80 * 25; i++) {
        screen_buffer[i] = (0x0F << 8) | ' ';
    }
    cursor_x = 0;
    cursor_y = 0;
}

void print(const char *text) {
    volatile uint16_t *screen_buffer = (volatile uint16_t*)0xB8000;

    for (int i = 0; text[i] != '\0'; i++) {
        
        if (text[i] == '\n') {
            cursor_x = 0;
            cursor_y++;
        } else {
            int offset = cursor_y * 80 + cursor_x;
            screen_buffer[offset] = (0x0F << 8) | (uint8_t)text[i];
            cursor_x++;
            
            if (cursor_x >= 80) {
                cursor_x = 0;
                cursor_y++;
            }
        }

        
        if (cursor_y >= 25) {
            print_clear();
        }
    }
}