// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------
// READ "Sound and KlibSound - User Manual" atomglide.com/lib


#include "program/blaster/blaster.h"
#include "component/windows_system/win.h"
#include "component/graphics_system/colors.h"
#include "component/graphics_system/grap.h"
#include "component/graphics_system/font.h"
#include "klib.h"


static int measure_text_width(const char *str) {
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


static void draw_panel(win_context_t *ctx, int x, int y, int w, int h,
                        uint32_t fill_color, uint32_t border_color) {
    draw_rect_canvas(ctx, x, y, w, h, border_color);
    draw_rect_canvas(ctx, x + 1, y + 1, w - 2, h - 2, fill_color);
}

static void draw_centered_text(win_context_t *ctx, int x, int y, int w, int h,
                                const char *str, uint32_t color) {
    int text_w = measure_text_width(str);
    int tx = x + (w - text_w) / 2;
    int ty = y + h / 2 + 5;
    draw_string_canvas(ctx, tx, ty, str, color);
}

static const int sin_table[32] = {
    0, 19, 38, 55, 70, 83, 92, 98, 100, 98, 92, 83, 70, 55, 38, 19,
    0, -19, -38, -55, -70, -83, -92, -98, -100, -98, -92, -83, -70, -55, -38, -19
};


#define HOLD_WATCHDOG_MS 150

typedef struct {
    int freq;          
    int time_ms;        
    int range_mode;    
    int time_offset;    
    int hold_mode;      
    char active_key[8]; 
} blaster_state_t;

static blaster_state_t state;

static void draw_vert_line(win_context_t *ctx, int x, int y1, int y2, uint32_t color) {
    if (x < 0 || x >= ctx->width) return;
    int start = y1 < y2 ? y1 : y2;
    int end = y1 > y2 ? y1 : y2;
    for (int py = start; py <= end; py++) {
        if (py >= 0 && py < ctx->height) {
            ctx->buffer[py * ctx->width + x] = color;
        }
    }
}

static void blaster_render(win_context_t *ctx) {
    for (int i = 0; i < ctx->width * ctx->height; i++) {
        ctx->buffer[i] = 0x0018181B; 
    }

    int osc_x = 20, osc_y = 20, osc_w = 400, osc_h = 250;
    int origin_x = osc_x + 30; 
    int origin_y = osc_y + osc_h / 2; 

    draw_rect_canvas(ctx, osc_x, osc_y, osc_w, osc_h, 0x00333333); 
    
    draw_rect_canvas(ctx, osc_x, origin_y, osc_w, 1, 0x00A0A0A0); 
    draw_rect_canvas(ctx, origin_x, osc_y, 1, osc_h, 0x00A0A0A0); 

    draw_string_canvas(ctx, origin_x + 5, osc_y + 5, "y", 0x00A0A0A0);
    draw_string_canvas(ctx, osc_x + osc_w - 15, origin_y + 5, "x", 0x00A0A0A0);
    draw_string_canvas(ctx, origin_x - 15, origin_y + 5, "0", 0x00A0A0A0);
    
    int prev_y = -1;
    for (int px = origin_x; px < osc_x + osc_w; px++) {
        int y = origin_y;
        if (state.freq > 0) {
            int visual_period = 15000 / state.freq; 
            if (visual_period < 5) visual_period = 5;
            
            int phase = (px - origin_x + state.time_offset) % visual_period;
            int phase_scaled = (phase * 32 * 256) / visual_period;
            int idx1 = (phase_scaled / 256) % 32;
            int idx2 = (idx1 + 1) % 32;
            int frac = phase_scaled % 256;
            
            int val = sin_table[idx1] + ((sin_table[idx2] - sin_table[idx1]) * frac) / 256;
            
            y = origin_y - (val * (osc_h / 2 - 15)) / 100;
        }
        if (prev_y != -1) {
            draw_vert_line(ctx, px, prev_y, y, 0x00EF4444); 
        }
        prev_y = y;
    }

    char buf[32];
    int panel_x = 440;
    
    draw_panel(ctx, panel_x, 20, 200, 60, 0x00EF4444, COLOR_WHITE);
    itoa(state.freq, buf);
    draw_string_canvas(ctx, panel_x + 12, 40, "Freq:", COLOR_WHITE);
    draw_string_canvas(ctx, panel_x + 75, 40, buf, COLOR_WHITE);
    draw_string_canvas(ctx, panel_x + 155, 40, "Hz", COLOR_WHITE);
    draw_panel(ctx, panel_x, 90, 200, 60, 0x00EF4444, COLOR_WHITE);
    itoa(state.time_ms, buf);
    draw_string_canvas(ctx, panel_x + 12, 110, "Time:", COLOR_WHITE);
    draw_string_canvas(ctx, panel_x + 75, 110, buf, COLOR_WHITE);
    draw_string_canvas(ctx, panel_x + 155, 110, "ms", COLOR_WHITE);
    draw_panel(ctx, panel_x, 160, 200, 60, 0x00EF4444, COLOR_WHITE);
    draw_string_canvas(ctx, panel_x + 12, 180, "Key:", COLOR_WHITE);
    draw_string_canvas(ctx, panel_x + 75, 180, state.active_key, COLOR_WHITE);

    int times[] = {200, 400, 500, 700, 1000};
    for (int i = 0; i < 5; i++) {
        int bx = panel_x + (i * 41);
        int active = (state.time_ms == times[i]);
        uint32_t fill = active ? 0x00EF4444 : 0x00404040;
        uint32_t border = active ? COLOR_WHITE : 0x00707070;
        draw_panel(ctx, bx, 230, 36, 40, fill, border);
        itoa(times[i], buf);
        draw_centered_text(ctx, bx, 230, 36, 40, buf, COLOR_WHITE);
    }

    {
        int active10 = (state.range_mode == 1);
        draw_panel(ctx, 20, 290, 120, 35,
                   active10 ? 0x00EF4444 : 0x00404040,
                   active10 ? COLOR_WHITE : 0x00707070);
        draw_centered_text(ctx, 20, 290, 120, 35, "10-90 Hz", COLOR_WHITE);

        int active100 = (state.range_mode == 0);
        draw_panel(ctx, 150, 290, 120, 35,
                   active100 ? 0x00EF4444 : 0x00404040,
                   active100 ? COLOR_WHITE : 0x00707070);
        draw_centered_text(ctx, 150, 290, 120, 35, "100-900 Hz", COLOR_WHITE);
    }

    {
        uint32_t fill = state.hold_mode ? 0x00EF4444 : 0x00404040;
        uint32_t border = state.hold_mode ? COLOR_WHITE : 0x00707070;
        draw_panel(ctx, 20, 335, 250, 35, fill, border);
        draw_centered_text(ctx, 20, 335, 250, 35,
                            state.hold_mode ? "Mode: HOLD" : "Mode: IMPULSE",
                            COLOR_WHITE);
    }

    draw_string_canvas(ctx, 20, 405, "Keys 1-9 = Set Freq, 0 = Stop", 0x00A0A0A0);
}

void blaster_init(win_context_t *ctx) {
    state.freq = 0;
    state.time_ms = 200;
    state.range_mode = 0; 
    state.time_offset = 0;
    state.hold_mode = 0; 
    kstrcpy(state.active_key, "None");

    if (ctx && ctx->id != -1) {
        blaster_render(ctx);
    }
}

void blaster_tick(void) {
    win_context_t ctx = thiswindowname("BLASTER");
    if (ctx.id != -1 && ctx.buffer) {
        if (state.freq > 0) {
            state.time_offset += (state.freq / 50) + 1;
        }
        blaster_render(&ctx);
        wm_update_window(&ctx);
    }
}

void blaster_event_handler(win_context_t *ctx, wm_event_t *evt) {
    if (!ctx || ctx->id == -1) return;

    if (evt->type == WM_EVENT_MOUSE) {
        int mx = evt->param1;
        int my = evt->param2;
        int panel_x = 440;
        int times[] = {200, 400, 500, 700, 1000};
        for (int i = 0; i < 5; i++) {
            if (mx >= panel_x + (i * 41) && mx <= panel_x + 36 + (i * 41) && my >= 230 && my <= 270) {
                state.time_ms = times[i];
            }
        }
        
        if (mx >= 20 && mx <= 140 && my >= 290 && my <= 325) state.range_mode = 1;  // 10-90Hz
        if (mx >= 150 && mx <= 270 && my >= 290 && my <= 325) state.range_mode = 0; // 100-900Hz

        if (mx >= 20 && mx <= 270 && my >= 335 && my <= 370) {
            state.hold_mode = !state.hold_mode;
            sound_stop();
        }
    }

    if (evt->type == WM_EVENT_KEY) {
        uint32_t k = evt->param1;
        
        if (k >= '1' && k <= '9') {
            int multiplier = (state.range_mode == 0) ? 100 : 10;
            state.freq = (k - '0') * multiplier;
            
            state.active_key[0] = (char)k;
            state.active_key[1] = '\0';

            if (!state.hold_mode) {
            
                sound_start(state.freq, state.time_ms);
            } else {
      
                sound_start(state.freq, HOLD_WATCHDOG_MS);
            }
        } 
      
        else if (k == '0') {
            state.freq = 0;
            kstrcpy(state.active_key, "None");
            sound_stop(); 
        }
    }

    blaster_render(ctx);
}