// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------



#include "ui_button.h"
#include "component/graphics_system/grap.h"
#include "component/graphics_system/colors.h"
#include "component/graphics_system/font.h"


static int ui_button_measure_text(const char *str) {
    const GFXfont *font = &FreeSans9pt7b;
    int width = 0;
    while (*str) {
        uint8_t uc = (uint8_t)*str;
        if (uc >= font->first && uc <= font->last) {
            width += font->glyph[uc - font->first].xAdvance;
        }
        str++;
    }
    return width;
}

void ui_button_init(ui_button_t *btn, int x, int y, int w, int h, const char *label) {
    btn->x = x;
    btn->y = y;
    btn->width = w;
    btn->height = h;
    btn->label = label;
    btn->bg_color = COLOR_THINKPAD_RED;
    btn->text_color = COLOR_WHITE;
    btn->border_color = COLOR_WHITE;
    btn->active_bg_color = COLOR_WHITE;
    btn->active_text_color = COLOR_BLACK;
    btn->active_border_color = COLOR_THINKPAD_RED;
    
    btn->is_pressed = 0;
}

int ui_button_contains(const ui_button_t *btn, int px, int py) {
    return (px >= btn->x && px < btn->x + btn->width &&
            py >= btn->y && py < btn->y + btn->height);
}

void ui_button_draw(const ui_button_t *btn) {
    uint32_t bg = btn->is_pressed ? btn->active_bg_color : btn->bg_color;
    uint32_t fg = btn->is_pressed ? btn->active_text_color : btn->text_color;
    uint32_t border = btn->is_pressed ? btn->active_border_color : btn->border_color;

    draw_rect(btn->x, btn->y, btn->width, btn->height, border);
    draw_rect(btn->x + 1, btn->y + 1, btn->width - 2, btn->height - 2, bg);

   
    int text_w = ui_button_measure_text(btn->label);
    int text_x = btn->x + (btn->width - text_w) / 2;
    int text_y = btn->y + (btn->height + 5) / 2;

   
    fb_draw_string(text_x, text_y, btn->label, fg, bg);
}

int ui_button_handle_click(ui_button_t *btn, int click_x, int click_y, uint8_t is_pressed) {
    if (ui_button_contains(btn, click_x, click_y)) {
        btn->is_pressed = is_pressed;
        ui_button_draw(btn);
        return 1;
    }
    return 0;
}