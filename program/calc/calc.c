// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------


#include "program/calc/calc.h"
#include "component/windows_system/win.h"
#include "component/graphics_system/colors.h"
#include "component/graphics_system/font.h"

#define MAX_DISP 16

static int32_t val1 = 0;
static int32_t val2 = 0;
static char current_op = '\0';
static int new_entry = 1;
static char disp[MAX_DISP] = "0";
static int disp_len = 1;

static void int_to_str(int32_t n, char *buf) {
    if (n == 0) { buf[0] = '0'; buf[1] = '\0'; return; }
    int i = 0, is_neg = 0;
    if (n < 0) { is_neg = 1; n = -n; }
    char tmp[12]; int t = 0;
    while (n > 0) { tmp[t++] = '0' + (n % 10); n /= 10; }
    if (is_neg) buf[i++] = '-';
    while (t > 0) buf[i++] = tmp[--t];
    buf[i] = '\0';
}

static int32_t str_to_int(const char *buf) {
    int32_t res = 0; int sign = 1, i = 0;
    if (buf[0] == '-') { sign = -1; i = 1; }
    while (buf[i] >= '0' && buf[i] <= '9') {
        res = res * 10 + (buf[i] - '0');
        i++;
    }
    return res * sign;
}

static void draw_rect_canvas(win_context_t *ctx, int x, int y, int w, int h, uint32_t color) {
    for (int cy = y; cy < y + h; cy++) {
        if (cy < 0 || cy >= ctx->height) continue;
        for (int cx = x; cx < x + w; cx++) {
            if (cx < 0 || cx >= ctx->width) continue;
            ctx->buffer[cy * ctx->width + cx] = color;
        }
    }
}

static int draw_char_canvas(win_context_t *ctx, int x, int y, char c, uint32_t fg) {
    uint8_t uc = (uint8_t)c;
    const GFXfont *font = &FreeSans9pt7b;
    if (uc < font->first || uc > font->last) return 0;
    GFXglyph *glyph = &font->glyph[uc - font->first];
    uint8_t *bitmap = font->bitmap;
    uint16_t bo = glyph->bitmapOffset;
    uint8_t w = glyph->width, h = glyph->height;
    int8_t xo = glyph->xOffset, yo = glyph->yOffset;
    uint8_t bit = 0; uint16_t bits = 0;
    for (int yy = 0; yy < h; yy++) {
        for (int xx = 0; xx < w; xx++) {
            if (!(bit++ & 7)) bits = bitmap[bo++];
            if (bits & 0x80) {
                int px = x + xo + xx, py = y + yo + yy;
                if (px >= 0 && px < ctx->width && py >= 0 && py < ctx->height) {
                    ctx->buffer[py * ctx->width + px] = fg;
                }
            }
            bits <<= 1;
        }
    }
    return glyph->xAdvance;
}

static void draw_str_canvas(win_context_t *ctx, int x, int y, const char *str, uint32_t color) {
    int cx = x;
    while (*str) {
        int adv = draw_char_canvas(ctx, cx, y, *str, color);
        cx += (adv ? adv : 8);
        str++;
    }
}

static void calc_render(win_context_t *ctx) {
    draw_rect_canvas(ctx, 0, 0, ctx->width, ctx->height, 0x00111111);

    int disp_h = 36;
    draw_rect_canvas(ctx, 10, 10, ctx->width - 20, disp_h, 0x000F172A);
    draw_rect_canvas(ctx, 10, 10 + disp_h, ctx->width - 20, 1, COLOR_THINKPAD_RED);
    draw_str_canvas(ctx, 20, 34, disp, COLOR_WHITE);

    const char *keys[4][4] = {
        {"7", "8", "9", "/"},
        {"4", "5", "6", "*"},
        {"1", "2", "3", "-"},
        {"C", "0", "=", "+"}
    };

    int start_y = 56, btn_gap = 6;
    int btn_w = (ctx->width - 20 - (btn_gap * 3)) / 4;
    int btn_h = (ctx->height - start_y - 10 - (btn_gap * 3)) / 4;
    if (btn_w < 10 || btn_h < 10) return;

    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            int bx = 10 + c * (btn_w + btn_gap);
            int by = start_y + r * (btn_h + btn_gap);
            uint32_t btn_bg = (c == 3) ? COLOR_THINKPAD_RED : 0x00222222;
            if (r == 3 && c == 0) btn_bg = 0x007F1D1D;
            draw_rect_canvas(ctx, bx, by, btn_w, btn_h, btn_bg);
            draw_str_canvas(ctx, bx + (btn_w / 2) - 4, by + (btn_h / 2) + 6, keys[r][c], COLOR_WHITE);
        }
    }
}

static void calc_input_digit(char d) {
    if (new_entry) {
        disp[0] = d; disp[1] = '\0'; disp_len = 1; new_entry = 0;
    } else if (disp_len < MAX_DISP - 1) {
        if (disp_len == 1 && disp[0] == '0') disp[0] = d;
        else { disp[disp_len++] = d; disp[disp_len] = '\0'; }
    }
}

static void calc_compute(void) {
    if ('\0' == current_op) return;
    val2 = str_to_int(disp);
    int32_t res = 0;
    switch (current_op) {
        case '+': res = val1 + val2; break;
        case '-': res = val1 - val2; break;
        case '*': res = val1 * val2; break;
        case '/': 
            if (val2 != 0) res = val1 / val2; 
            else {
                disp[0] = 'E'; disp[1] = 'r'; disp[2] = 'r'; disp[3] = '\0';
                disp_len = 3; new_entry = 1; current_op = '\0';
                return;
            }
            break;
    }
    int_to_str(res, disp);
    disp_len = 0;
    while (disp[disp_len] != '\0') disp_len++;
    new_entry = 1; current_op = '\0';
}

static void calc_op(char op) {
    val1 = str_to_int(disp);
    current_op = op;
    new_entry = 1;
}

void calc_init(win_context_t *ctx) {
    disp[0] = '0'; disp[1] = '\0'; disp_len = 1;
    val1 = 0; val2 = 0; current_op = '\0'; new_entry = 1;
    calc_render(ctx);
}

void calc_event_handler(win_context_t *ctx, wm_event_t *evt) {
    if (evt->type == WM_EVENT_REDRAW) {
        calc_render(ctx);
        return;
    }

    // ОБРАБОТКА КЛИКА МЫШИ ПО КНОПКАМ КАЛЬКУЛЯТОРА
    if (evt->type == WM_EVENT_MOUSE) {
        int mx = (int)evt->param1;
        int my = (int)evt->param2;

        int start_y = 56, btn_gap = 6;
        int btn_w = (ctx->width - 20 - (btn_gap * 3)) / 4;
        int btn_h = (ctx->height - start_y - 10 - (btn_gap * 3)) / 4;

        if (btn_w > 0 && btn_h > 0 && my >= start_y && mx >= 10 && mx < ctx->width - 10) {
            const char *keys[4][4] = {
                {"7", "8", "9", "/"},
                {"4", "5", "6", "*"},
                {"1", "2", "3", "-"},
                {"C", "0", "=", "+"}
            };

            for (int r = 0; r < 4; r++) {
                for (int c = 0; c < 4; c++) {
                    int bx = 10 + c * (btn_w + btn_gap);
                    int by = start_y + r * (btn_h + btn_gap);
                    if (mx >= bx && mx < bx + btn_w && my >= by && my < by + btn_h) {
                        char k = keys[r][c][0];
                        if (k >= '0' && k <= '9') calc_input_digit(k);
                        else if (k == '+' || k == '-' || k == '*' || k == '/') calc_op(k);
                        else if (k == '=') calc_compute();
                        else if (k == 'C') calc_init(ctx);

                        calc_render(ctx);
                        wm_update_window(ctx);
                        return;
                    }
                }
            }
        }
    }

    if (evt->type == WM_EVENT_KEY) {
        char k = (char)evt->param1;
        if (k >= '0' && k <= '9') calc_input_digit(k);
        else if (k == '+' || k == '-' || k == '*' || k == '/') calc_op(k);
        else if (k == '=' || k == '\n') calc_compute();
        else if (k == 'c' || k == 'C') {
            calc_init(ctx);
            wm_update_window(ctx);
            return;
        } else if (k == '\b') {
            if (disp_len > 1) { disp_len--; disp[disp_len] = '\0'; }
            else { disp[0] = '0'; disp[1] = '\0'; disp_len = 1; }
        }
        calc_render(ctx);
        wm_update_window(ctx);
    }
}