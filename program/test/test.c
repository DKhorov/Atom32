// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------



#include "program/test/test.h"
#include "component/windows_system/win.h"
#include "component/graphics_system/colors.h"
#include "component/graphics_system/grap.h"
#include "component/app_registry/app_registry.h"
#include "klib.h"

typedef struct {
    int btn1_hover;
    int btn2_hover;
    int click_count;
    uint32_t active_color;
} test_app_state_t;

static test_app_state_t state;

static void test_app_render(win_context_t *ctx) {
    for (int i = 0; i < ctx->width * ctx->height; i++) {
        ctx->buffer[i] = 0x000F172A;
    }

    draw_string_canvas(ctx, 15, 20, "ATOM32 GRAPHICS TEST", COLOR_GREEN);
    draw_string_canvas(ctx, 15, 40, "Testing Canvas primitives & UI", COLOR_WHITE);
    draw_rect_canvas(ctx, 15, 60, 40, 30, COLOR_RED);
    draw_rect_canvas(ctx, 60, 60, 40, 30, COLOR_GREEN);
    draw_rect_canvas(ctx, 105, 60, 40, 30, COLOR_BLUE);
    draw_rect_canvas(ctx, 150, 60, 40, 30, state.active_color); // Динамический блок
    uint32_t btn1_color = state.btn1_hover ? 0x0038BDF8 : 0x000284C7;
    draw_rect_canvas(ctx, 15, 110, 100, 35, btn1_color);
    draw_string_canvas(ctx, 25, 132, "CLICK ME", COLOR_WHITE);
    uint32_t btn2_color = state.btn2_hover ? 0x00EF4444 : 0x00B91C1C;
    draw_rect_canvas(ctx, 125, 110, 90, 35, btn2_color);
    draw_string_canvas(ctx, 140, 132, "RESET", COLOR_WHITE);
    draw_string_canvas(ctx, 15, 175, "Clicks:", COLOR_WHITE);
    char clicks_str[16];
    itoa(state.click_count, clicks_str);
    draw_string_canvas(ctx, 80, 175, clicks_str, COLOR_GREEN);
}

void test_app_init(win_context_t *ctx) {
    state.btn1_hover = 0;
    state.btn2_hover = 0;
    state.click_count = 0;
    state.active_color = COLOR_WHITE;
    if (ctx && ctx->id != -1) {
        test_app_render(ctx);
    }
}

void test_app_tick(void) {
    win_context_t ctx = thiswindowname("TEST");
    if (ctx.id != -1 && ctx.buffer) {
        test_app_render(&ctx);
        wm_update_window(&ctx);
    }
}

void test_app_event_handler(win_context_t *ctx, wm_event_t *evt) {
    if (!ctx || ctx->id == -1) return;

    if (evt->type == WM_EVENT_MOUSE) {
        int mx = evt->param1;
        int my = evt->param2; 
        state.btn1_hover = (mx >= 15 && mx <= 115 && my >= 110 && my <= 145);
        state.btn2_hover = (mx >= 125 && mx <= 215 && my >= 110 && my <= 145);
        if (state.btn1_hover) {
            state.click_count++;
            state.active_color = (state.click_count % 2 == 0) ? COLOR_WHITE : 0x00F59E0B;
        } else if (state.btn2_hover) {
            state.click_count = 0;
            state.active_color = COLOR_WHITE;
        }
    }

    if (evt->type == WM_EVENT_KEY) {
        uint32_t k = evt->param1;
        if (k == ' ' || k == '\n') {
            state.click_count++;
        } else if (k == 'r' || k == 'R') {
            state.click_count = 0;
            state.active_color = COLOR_WHITE;
        }
    }

    test_app_render(ctx);
}