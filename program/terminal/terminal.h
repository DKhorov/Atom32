#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>
#include "component/windows_system/win.h"

void term_init(win_context_t *ctx);
void term_event_handler(win_context_t *ctx, wm_event_t *evt);
void term_putc(win_context_t *ctx, char c, uint32_t color);
void term_puts(win_context_t *ctx, const char *str, uint32_t color);
void term_scroll(win_context_t *ctx);
void term_clear_screen(win_context_t *ctx);
void term_toggle_theme(win_context_t *ctx);

int term_get_cursor_y(win_context_t *ctx);
void term_set_cursor_x(win_context_t *ctx, int x);
void term_set_cursor_y(win_context_t *ctx, int y);

#endif