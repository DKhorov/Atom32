// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------
// DogHouse IDE for Atom32




#include <stdint.h>
#include "program/doghouse/doghouse.h"
#include "component/windows_system/win.h"
#include "component/graphics_system/colors.h"
#include "component/graphics_system/font.h"
#include "component/graphics_system/grap.h"

#define MAX_TEXT_SIZE 2048

static char text_buf[MAX_TEXT_SIZE];
static int text_len = 0;



static void ctx_draw_rect(win_context_t *ctx, int x, int y, int w, int h, uint32_t color) {
    for (int cy = y; cy < y + h; cy++) {
        if (cy < 0 || cy >= ctx->height) continue;
        for (int cx = x; cx < x + w; cx++) {
            if (cx < 0 || cx >= ctx->width) continue;
            ctx->buffer[cy * ctx->width + cx] = color;
        }
    }
}

static void ctx_draw_string(win_context_t *ctx, int x, int y, const char *str, uint32_t color) {
    int cx = x;
    while (*str) {
        int adv = draw_char_canvas(ctx, cx, y, *str, color);
        if (adv == 0) adv = 8;
        cx += adv;
        str++;
    }
}

static void doghouse_render(win_context_t *ctx) {
    for (int i = 0; i < ctx->width * ctx->height; i++) {
        ctx->buffer[i] = COLOR_BLACK;
    }

    // Верхняя служебная плашка IDE
    ctx_draw_rect(ctx, 0, 0, ctx->width, 22, 0x001F2937);
    ctx_draw_string(ctx, 8, 16, "DogHouse IDE v0.1 - Editor", COLOR_WHITE);

    // Отрисовка текста редактора
    int rx = 8;
    int ry = 38;
    for (int i = 0; i < text_len; i++) {
        if (text_buf[i] == '\n') {
            rx = 8;
            ry += 16;
        } else {
            int adv = draw_char_canvas(ctx, rx, ry, text_buf[i], COLOR_WHITE);
            if (adv == 0) adv = 8;
            rx += adv;
            if (rx + 12 > ctx->width) {
                rx = 8;
                ry += 16;
            }
        }
    }
}

void doghouse_init(win_context_t *ctx) {
    text_len = 0;
    text_buf[0] = '\0';
    doghouse_render(ctx);
}

void doghouse_event_handler(win_context_t *ctx, wm_event_t *evt) {
    if (evt->type == WM_EVENT_REDRAW) {
        doghouse_render(ctx);
        return;
    }

    if (evt->type == WM_EVENT_KEY) {
        char c = (char)evt->param1;
        if (c == '\0') return;

        if (c == '\b') {
            if (text_len > 0) {
                text_len--;
                text_buf[text_len] = '\0';
                doghouse_render(ctx);
                wm_update_window(ctx);
            }
        } else if (text_len < MAX_TEXT_SIZE - 1) {
            text_buf[text_len++] = c;
            text_buf[text_len] = '\0';
            doghouse_render(ctx);
            wm_update_window(ctx);
        }
    }
}