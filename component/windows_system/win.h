// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------



#ifndef WIN_H
#define WIN_H

#include <stdint.h>

#define MAX_TILES 6
#define WM_EVENT_KEY 1
#define WM_EVENT_REDRAW 2
#define WM_EVENT_MOUSE 3
typedef struct {
    uint32_t type;
    uint32_t param1;
    uint32_t param2;
} wm_event_t;

struct win_context;

typedef void (*win_event_handler_t)(struct win_context *ctx, wm_event_t *evt);

typedef struct win_context {
    int id;
    int width;
    int height;
    uint32_t *buffer;
    win_event_handler_t on_event;
} win_context_t;

typedef struct {
    int id;
    const char *title;
    uint32_t bg_color;
    win_context_t ctx;
} tile_t;

void wm_init(void);
void wm_add_tile(const char *title, uint32_t bg_color, win_event_handler_t handler);
win_context_t thiswindowname(const char *title);
void wm_draw_all(void);
void wm_update_window(win_context_t *ctx);
void wm_draw_char_rect(win_context_t *ctx, int char_x, int char_y);
void wm_handle_mouse(int mouse_x, int mouse_y, uint8_t left_button);
void wm_dispatch_key(char key);

#endif