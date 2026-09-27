// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------



#include "point.h"
#include "component/graphics_system/grap.h"
#include "component/graphics_system/colors.h"

trackpoint_state_t g_tp = {0, 0, 0, 0, 0};

#define CURSOR_WIDTH  16
#define CURSOR_HEIGHT 16

static const uint8_t cursor_arrow[CURSOR_HEIGHT][CURSOR_WIDTH] = {
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 3, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 2, 3, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 2, 2, 3, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 2, 2, 2, 3, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 2, 2, 2, 2, 3, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 2, 2, 2, 2, 2, 3, 3, 1, 0, 0, 0, 0, 0, 0, 0},
    {1, 2, 2, 2, 2, 2, 2, 3, 3, 1, 0, 0, 0, 0, 0, 0},
    {1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 1, 0, 0, 0, 0, 0},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 1, 0, 0, 0, 0},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 1, 0, 0, 0},
    {1, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {1, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

static inline void outb(uint16_t port, uint8_t val) { __asm__ __volatile__ ("outb %0, %1" : : "a"(val), "Nd"(port)); }
static inline uint8_t inb(uint16_t port) { uint8_t ret; __asm__ __volatile__ ("inb %1, %0" : "=a"(ret) : "Nd"(port)); return ret; }

static void mouse_wait(uint8_t type) {
    uint32_t timeout = 100000;
    if (type == 0) while (timeout-- && !(inb(0x64) & 1));
    else while (timeout-- && (inb(0x64) & 2));
}

static void mouse_write(uint8_t data) {
    mouse_wait(1); outb(0x64, 0xD4);
    mouse_wait(1); outb(0x60, data);
}

static uint8_t mouse_read(void) {
    mouse_wait(0); return inb(0x60);
}

void trackpoint_init(void) {
    g_tp.x = fb_get_width() / 2;
    g_tp.y = fb_get_height() / 2;

    mouse_wait(1); outb(0x64, 0xA8);
    mouse_wait(1); outb(0x64, 0x20);
    mouse_wait(0);
    uint8_t status = inb(0x60);

    status |= 0x02;
    status &= ~0x20;

    mouse_wait(1); outb(0x64, 0x60);
    mouse_wait(1); outb(0x60, status);

    mouse_write(0xF6); mouse_read();
    mouse_write(0xF4); mouse_read();
}

void trackpoint_poll(void) {
    static uint8_t cycle = 0;
    static uint8_t buf[3];

    while (inb(0x64) & 0x01) {
        uint8_t status = inb(0x64);
        if (!(status & 0x20)) { inb(0x60); continue; }

        uint8_t b = inb(0x60);
        if (cycle == 0) {
            if (b & 0x08) { buf[0] = b; cycle = 1; }
        } else if (cycle == 1) {
            buf[1] = b; cycle = 2;
        } else if (cycle == 2) {
            buf[2] = b; cycle = 0;

            g_tp.left_button   = buf[0] & 0x01;
            g_tp.right_button  = (buf[0] >> 1) & 0x01;
            g_tp.middle_button = (buf[0] >> 2) & 0x01;

            int16_t rel_x = buf[1];
            int16_t rel_y = buf[2];
            if (buf[0] & 0x10) rel_x |= 0xFF00;
            if (buf[0] & 0x20) rel_y |= 0xFF00;

            if (buf[0] & 0xC0) continue;

            g_tp.x += rel_x;
            g_tp.y -= rel_y;

            int max_x = (int)fb_get_width() - CURSOR_WIDTH;
            int max_y = (int)fb_get_height() - CURSOR_HEIGHT;

            if (g_tp.x < 0) g_tp.x = 0;
            if (g_tp.y < 0) g_tp.y = 0;
            if (g_tp.x > max_x) g_tp.x = max_x;
            if (g_tp.y > max_y) g_tp.y = max_y;
        }
    }
}

void trackpoint_draw_direct(void) {
    for (int dy = 0; dy < CURSOR_HEIGHT; dy++) {
        for (int dx = 0; dx < CURSOR_WIDTH; dx++) {
            uint8_t pt = cursor_arrow[dy][dx];
            if (pt == 1) fb_put_pixel_direct(g_tp.x + dx, g_tp.y + dy, COLOR_BLACK);
            else if (pt == 2) fb_put_pixel_direct(g_tp.x + dx, g_tp.y + dy, COLOR_WHITE);
            else if (pt == 3) fb_put_pixel_direct(g_tp.x + dx, g_tp.y + dy, COLOR_THINKPAD_RED);
        }
    }
}