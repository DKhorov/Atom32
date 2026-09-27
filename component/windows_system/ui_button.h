// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------



#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include <stdint.h>

typedef struct {
    int x;
    int y;
    int width;
    int height;
    const char *label;
    uint32_t bg_color;
    uint32_t text_color;
    uint32_t border_color;
    uint32_t active_bg_color;
    uint32_t active_text_color;
    uint32_t active_border_color;
    int is_pressed;
} ui_button_t;

void ui_button_init(ui_button_t *btn, int x, int y, int w, int h, const char *label);
void ui_button_draw(const ui_button_t *btn);
int ui_button_contains(const ui_button_t *btn, int px, int py);
int ui_button_handle_click(ui_button_t *btn, int click_x, int click_y, uint8_t is_pressed);

#endif