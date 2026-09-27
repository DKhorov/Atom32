// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------

#include "alert.h"
#include "component/graphics_system/grap.h"
#include "component/graphics_system/colors.h"

static int log_y = 20;
#define LOG_X 30
#define LINE_HEIGHT 20

void syslog_init(void) {
    log_y = 20;
}

void syslog_info(const char *msg) {
    fb_draw_string(LOG_X, log_y, "[INFO] ", COLOR_GREEN, COLOR_BLACK);
    fb_draw_string(LOG_X + 56, log_y, msg, COLOR_WHITE, COLOR_BLACK);
    log_y += LINE_HEIGHT;
}

void syslog_ok(const char *msg) {
    fb_draw_string(LOG_X, log_y, "[ OK ] ", 0x0000FF00, COLOR_BLACK);
    fb_draw_string(LOG_X + 56, log_y, msg, COLOR_WHITE, COLOR_BLACK);
    log_y += LINE_HEIGHT;
}