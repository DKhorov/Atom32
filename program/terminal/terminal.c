// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------


#include "program/terminal/terminal.h"
#include "program/terminal/command.h"
#include "component/windows_system/win.h"
#include "component/graphics_system/colors.h"
#include "component/graphics_system/font.h"
#include "component/graphics_system/grap.h"
#include "klib.h"
#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef COLOR_CYAN
#define COLOR_CYAN 0x0006B6D4
#endif

#define TERM_MAX_INSTANCES 16

typedef struct {
    int cursor_x;
    int cursor_y;

    char input_buf[128];
    int input_len;

    uint32_t text_color;
    uint32_t prompt_color;
} term_instance_t;

static term_instance_t terms[TERM_MAX_INSTANCES];

static const char *prompt_str = "Atom32 =>";
static const char *prompt_hint =
    " Type a command... (view - open full list commands)";

static term_instance_t *get_term(win_context_t *ctx) {
    int id = (ctx->id >= 0 && ctx->id < TERM_MAX_INSTANCES)
                 ? ctx->id
                 : 0;

    return &terms[id];
}

int term_get_cursor_y(win_context_t *ctx) {
    return get_term(ctx)->cursor_y;
}

void term_set_cursor_x(win_context_t *ctx, int x) {
    get_term(ctx)->cursor_x = x;
}

void term_set_cursor_y(win_context_t *ctx, int y) {
    get_term(ctx)->cursor_y = y;
}



static void clear_char_canvas(
    win_context_t *ctx,
    int x,
    int y,
    int width
) {
    int line_height = FreeSans9pt7b.yAdvance;
    int start_y = y - line_height + 4;

    if (start_y < 0) {
        start_y = 0;
    }

    for (int cy = start_y;
         cy < start_y + line_height;
         cy++) {
        for (int cx = 0;
             cx < width + 2;
             cx++) {
            int px = x + cx;

            if (px >= 0 &&
                px < ctx->width &&
                cy >= 0 &&
                cy < ctx->height) {
                ctx->buffer[cy * ctx->width + px] =
                    COLOR_BLACK;
            }
        }
    }
}

static void clear_current_line(win_context_t *ctx) {
    term_instance_t *t = get_term(ctx);

    int line_height = FreeSans9pt7b.yAdvance;
    int start_y = t->cursor_y - line_height + 4;

    if (start_y < 0) {
        start_y = 0;
    }

    for (int y = start_y;
         y < start_y + line_height;
         y++) {
        if (y < 0 || y >= ctx->height) {
            continue;
        }

        for (int x = 0; x < ctx->width; x++) {
            ctx->buffer[y * ctx->width + x] =
                COLOR_BLACK;
        }
    }
}

static int get_text_width(
    win_context_t *ctx,
    const char *text
) {
    int width = 0;
    const GFXfont *font = &FreeSans9pt7b;

    while (*text) {
        uint8_t c = (uint8_t)*text++;

        if (c >= font->first && c <= font->last) {
            width += font->glyph[c - font->first].xAdvance;
        } else {
            width += 8;
        }
    }

    return width;
}

void term_scroll(win_context_t *ctx) {
    term_instance_t *t = get_term(ctx);

    int line_height = FreeSans9pt7b.yAdvance;
    int scroll_pixels = ctx->width * line_height;
    int total_pixels = ctx->width * ctx->height;

    for (int i = 0;
         i < total_pixels - scroll_pixels;
         i++) {
        ctx->buffer[i] =
            ctx->buffer[i + scroll_pixels];
    }

    for (int i = total_pixels - scroll_pixels;
         i < total_pixels;
         i++) {
        ctx->buffer[i] = COLOR_BLACK;
    }

    t->cursor_y -= line_height;
}

void term_clear_screen(win_context_t *ctx) {
    term_instance_t *t = get_term(ctx);

    for (int i = 0;
         i < ctx->width * ctx->height;
         i++) {
        ctx->buffer[i] = COLOR_BLACK;
    }

    t->cursor_x = 0;
    t->cursor_y = FreeSans9pt7b.yAdvance;
}

void term_toggle_theme(win_context_t *ctx) {
    term_instance_t *t = get_term(ctx);

    if (t->text_color == COLOR_WHITE) {
        t->text_color = COLOR_CYAN;
        t->prompt_color = COLOR_THINKPAD_RED;

        term_puts(
            ctx,
            "Switched to Cyberpunk Cyan Theme\n",
            COLOR_CYAN
        );
    } else if (t->text_color == COLOR_CYAN) {
        t->text_color = COLOR_GREEN;
        t->prompt_color = COLOR_GREEN;

        term_puts(
            ctx,
            "Switched to Hacker Green CRT Theme\n",
            COLOR_GREEN
        );
    } else {
        t->text_color = COLOR_WHITE;
        t->prompt_color = COLOR_CYAN;

        term_puts(
            ctx,
            "Switched to Classic Atom Theme\n",
            COLOR_WHITE
        );
    }
}

void term_putc(
    win_context_t *ctx,
    char c,
    uint32_t color
) {
    term_instance_t *t = get_term(ctx);
    int line_height = FreeSans9pt7b.yAdvance;

    if (c == '\n') {
        t->cursor_x = 0;
        t->cursor_y += line_height;
    } else if (c == '\b') {
        int adv = 8;

        if (t->input_len > 0) {
            uint8_t uc =
                (uint8_t)t->input_buf[t->input_len - 1];

            if (uc >= FreeSans9pt7b.first &&
                uc <= FreeSans9pt7b.last) {
                adv = FreeSans9pt7b
                    .glyph[uc - FreeSans9pt7b.first]
                    .xAdvance;
            }
        }

        if (t->cursor_x >= adv) {
            t->cursor_x -= adv;

            clear_char_canvas(
                ctx,
                t->cursor_x,
                t->cursor_y,
                adv
            );
        }
    } else {
        int advance = draw_char_canvas(
            ctx,
            t->cursor_x,
            t->cursor_y,
            c,
            color
        );

        if (advance == 0) {
            advance = 8;
        }

        t->cursor_x += advance;

        if (t->cursor_x + 10 > ctx->width) {
            t->cursor_x = 0;
            t->cursor_y += line_height;
        }
    }

    if (t->cursor_y + 10 > ctx->height) {
        term_scroll(ctx);
    }
}

void term_puts(
    win_context_t *ctx,
    const char *str,
    uint32_t color
) {
    while (*str) {
        term_putc(ctx, *str++, color);
    }
}


static void draw_input_line(win_context_t *ctx) {
    term_instance_t *t = get_term(ctx);

    t->cursor_x = 0;

    term_puts(
        ctx,
        prompt_str,
        t->prompt_color
    );

    if (t->input_len > 0) {
        term_puts(
            ctx,
            t->input_buf,
            t->text_color
        );
    } else {
        term_puts(
            ctx,
            prompt_hint,
            0x00777777
        );
    }
}

static void redraw_input_line(win_context_t *ctx) {
    clear_current_line(ctx);
    draw_input_line(ctx);
}

static void term_redraw(win_context_t *ctx) {
    term_instance_t *t = get_term(ctx);

    for (int i = 0;
         i < ctx->width * ctx->height;
         i++) {
        ctx->buffer[i] = COLOR_BLACK;
    }

    t->cursor_x = 0;
    t->cursor_y = FreeSans9pt7b.yAdvance;

    term_puts(
        ctx,
        "Atom32 - Bare-metal OS - Atom Family Commands x86\n",
        COLOR_GREEN
    );

    term_puts(
        ctx,
        "Copyright (C) AtomGlide Labs 2025-2026 - Atom Family OS\n\n",
        0x00888888
    );

    draw_input_line(ctx);
}

static void execute_command(win_context_t *ctx) {
    term_instance_t *t = get_term(ctx);

    term_putc(ctx, '\n', COLOR_WHITE);

    if (t->input_len > 0) {
        terminal_execute_command(
            ctx,
            t->input_buf
        );
    }

    t->input_len = 0;
    t->input_buf[0] = '\0';

    term_puts(
        ctx,
        prompt_str,
        t->prompt_color
    );

    term_puts(
        ctx,
        prompt_hint,
        0x00777777
    );
}

void term_init(win_context_t *ctx) {
    term_instance_t *t = get_term(ctx);

    t->cursor_x = 0;
    t->cursor_y = FreeSans9pt7b.yAdvance;

    t->input_len = 0;
    t->input_buf[0] = '\0';

    t->text_color = COLOR_WHITE;
    t->prompt_color = COLOR_CYAN;

    term_redraw(ctx);
}

void term_event_handler(
    win_context_t *ctx,
    wm_event_t *evt
) {
    term_instance_t *t = get_term(ctx);

    if (evt->type == WM_EVENT_REDRAW) {
        term_redraw(ctx);
        wm_update_window(ctx);
        return;
    }

    if (evt->type != WM_EVENT_KEY) {
        return;
    }

    char c = (char)evt->param1;

    if (c == '\0') {
        return;
    }

    if (c == '\n') {
        t->input_buf[t->input_len] = '\0';

        execute_command(ctx);

        wm_update_window(ctx);
    } else if (c == '\b') {
        if (t->input_len > 0) {
            t->input_len--;
            t->input_buf[t->input_len] = '\0';

           
            redraw_input_line(ctx);

            wm_update_window(ctx);
        }
    } else if (t->input_len < 127) {
      

        if (t->input_len == 0) {
            clear_current_line(ctx);

            t->cursor_x = 0;

            term_puts(
                ctx,
                prompt_str,
                t->prompt_color
            );
        }

        t->input_buf[t->input_len++] = c;
        t->input_buf[t->input_len] = '\0';

        term_putc(
            ctx,
            c,
            t->text_color
        );

        wm_draw_char_rect(
            ctx,
            t->cursor_x,
            t->cursor_y
        );

        wm_update_window(ctx);
    }
}
