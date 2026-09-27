// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------



#include <stdint.h>
#include "component/graphics_system/grap.h"
#include "component/graphics_system/colors.h"
#include "component/graphics_system/font.h"
#include "component/windows_system/win.h"
#include "program/sysmon/sysmon.h"

#define MAX_LOG_LINES 12
#define MAX_LINE_LEN  64

static char log_buffer[MAX_LOG_LINES][MAX_LINE_LEN];
static int log_count = 0;



void sysmon_draw(win_context_t *ctx) {
    if (!ctx || ctx->id == -1 || !ctx->buffer) return;

    int total_pixels = ctx->width * ctx->height;
    for (int i = 0; i < total_pixels; i++) {
        ctx->buffer[i] = 0x000F172A;
    }

    if (ctx->height > 40) {
        draw_string_canvas(ctx, 15, 25, "ATOM32 SYSTEM MONITOR", COLOR_GREEN);
    }
    if (ctx->height > 60) {
        draw_string_canvas(ctx, 15, 50, "Status: System Ready", COLOR_WHITE);
    }

    // 3. Отрисовка логов с контролем вылета за нижнюю границу окна
    int y_pos = 75;
    for (int i = 0; i < log_count; i++) {
        if (y_pos + 10 < ctx->height) {
            draw_string_canvas(ctx, 15, y_pos, log_buffer[i], COLOR_WHITE);
        }
        y_pos += 18;
    }

    wm_update_window(ctx);
}

void sysmon_print(const char *msg) {
    if (!msg) return;

    if (log_count < MAX_LOG_LINES) {
        int i = 0;
        while (msg[i] != '\0' && i < MAX_LINE_LEN - 1) {
            log_buffer[log_count][i] = msg[i];
            i++;
        }
        log_buffer[log_count][i] = '\0';
        log_count++;
    } else {
        for (int k = 0; k < MAX_LOG_LINES - 1; k++) {
            int j = 0;
            while (log_buffer[k + 1][j] != '\0') {
                log_buffer[k][j] = log_buffer[k + 1][j];
                j++;
            }
            log_buffer[k][j] = '\0';
        }
        int i = 0;
        while (msg[i] != '\0' && i < MAX_LINE_LEN - 1) {
            log_buffer[MAX_LOG_LINES - 1][i] = msg[i];
            i++;
        }
        log_buffer[MAX_LOG_LINES - 1][i] = '\0';
    }

    win_context_t ctx = thiswindowname("SYSTEM MONITOR [1]");
    if (ctx.id != -1 && ctx.buffer) {
        sysmon_draw(&ctx);
    }
}

void sysmon_event_handler(win_context_t *ctx, wm_event_t *evt) {
    (void)evt;
    if (ctx && ctx->id != -1) {
        sysmon_draw(ctx);
    }
}