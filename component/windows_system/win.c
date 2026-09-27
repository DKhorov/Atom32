// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------



#include "win.h"
#include "component/app_registry/app_registry.h"
#include "component/graphics_system/grap.h"
#include "component/graphics_system/colors.h"
#include "component/graphics_system/font.h"
#include "component/windows_system/ui_button.h"

#define CANVAS_MAX_W 1366
#define CANVAS_MAX_H 768
#define TOP_BAR_H 32

static int show_system_info_modal = 0;

static tile_t tiles[MAX_TILES];
static int tile_count = 0;
static int active_tile_idx = -1;

static uint32_t buffer_memory[MAX_TILES][CANVAS_MAX_W * CANVAS_MAX_H];
static uint32_t *tile_buffers[MAX_TILES];


#define DEFAULT_WIN_W 720
#define DEFAULT_WIN_H 480

static int tile_x[MAX_TILES];
static int tile_y[MAX_TILES];
static int tile_w[MAX_TILES];
static int tile_h[MAX_TILES];
static int saved_x[MAX_TILES];
static int saved_y[MAX_TILES];
static int saved_w[MAX_TILES];
static int saved_h[MAX_TILES];
static int is_maximized[MAX_TILES];
static int drag_active = 0;
static int drag_tile_idx = -1;
static int drag_offset_x = 0;
static int drag_offset_y = 0;
static int wireframe_x = 0;
static int wireframe_y = 0;

static int kstrcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

static void get_tile_bounds(int index, int count, int *x, int *y, int *w, int *h) {
    (void)count;
    *x = tile_x[index];
    *y = tile_y[index];
    *w = tile_w[index];
    *h = tile_h[index];
}

static void draw_filled_circle(int cx, int cy, int radius, uint32_t color) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                fb_put_pixel(cx + x, cy + y, color);
            }
        }
    }
}

static void draw_wireframe_rect(int x, int y, int w, int h, uint32_t color) {
    // Внешняя и внутренняя линия для контрастности рамки
    draw_rect(x, y, w, 2, color);                  // верхняя
    draw_rect(x, y + h - 2, w, 2, color);          // нижняя
    draw_rect(x, y, 2, h, color);                  // левая
    draw_rect(x + w - 2, y, 2, h, color);          // правая
    draw_rect(x, y + 24, w, 2, color);             // под заголовком
}

static void wm_bring_to_front(int idx) {
    if (idx < 0 || idx >= tile_count) return;
    if (idx == tile_count - 1) {
        active_tile_idx = idx;
        return;
    }

    tile_t moved_tile = tiles[idx];
    uint32_t *moved_buf = tile_buffers[idx];
    int mx = tile_x[idx], my = tile_y[idx], mw = tile_w[idx], mh = tile_h[idx];
    int sx = saved_x[idx], sy = saved_y[idx], sw = saved_w[idx], sh = saved_h[idx];
    int max_st = is_maximized[idx];

    for (int i = idx; i < tile_count - 1; i++) {
        tiles[i] = tiles[i + 1];
        tiles[i].id = i;
        tiles[i].ctx.id = i;
        tile_buffers[i] = tile_buffers[i + 1];
        tile_x[i] = tile_x[i + 1];
        tile_y[i] = tile_y[i + 1];
        tile_w[i] = tile_w[i + 1];
        tile_h[i] = tile_h[i + 1];
        saved_x[i] = saved_x[i + 1];
        saved_y[i] = saved_y[i + 1];
        saved_w[i] = saved_w[i + 1];
        saved_h[i] = saved_h[i + 1];
        is_maximized[i] = is_maximized[i + 1];
    }

    int last = tile_count - 1;
    tiles[last] = moved_tile;
    tiles[last].id = last;
    tiles[last].ctx.id = last;
    tiles[last].ctx.buffer = moved_buf;
    tile_buffers[last] = moved_buf;
    tile_x[last] = mx;
    tile_y[last] = my;
    tile_w[last] = mw;
    tile_h[last] = mh;
    saved_x[last] = sx;
    saved_y[last] = sy;
    saved_w[last] = sw;
    saved_h[last] = sh;
    is_maximized[last] = max_st;

    active_tile_idx = last;
}

void wm_init(void) {
    tile_count = 0;
    active_tile_idx = -1;
    show_system_info_modal = 0;
    drag_active = 0;
    drag_tile_idx = -1;
    for (int i = 0; i < MAX_TILES; i++) {
        tile_buffers[i] = buffer_memory[i];
        is_maximized[i] = 0;
    }
}

void wm_add_tile(const char *title, uint32_t bg_color, win_event_handler_t handler) {
    if (tile_count >= MAX_TILES) return;
    int slot = tile_count;

    tiles[slot].id = slot;
    tiles[slot].title = title;
    tiles[slot].bg_color = bg_color;
    tiles[slot].ctx.on_event = handler;

    int screen_w = (int)fb_get_width();
    int screen_h = (int)fb_get_height();

    int cascade = (slot * 28) % 220;
    tile_w[slot] = DEFAULT_WIN_W;
    tile_h[slot] = DEFAULT_WIN_H;
    tile_x[slot] = 60 + cascade;
    tile_y[slot] = TOP_BAR_H + 40 + cascade;

    if (tile_x[slot] + tile_w[slot] > screen_w) tile_x[slot] = screen_w - tile_w[slot] - 10;
    if (tile_y[slot] + tile_h[slot] > screen_h) tile_y[slot] = screen_h - tile_h[slot] - 10;
    if (tile_x[slot] < 0) tile_x[slot] = 10;
    if (tile_y[slot] < TOP_BAR_H) tile_y[slot] = TOP_BAR_H + 10;

    is_maximized[slot] = 0;

    int canvas_w = (tile_w[slot] > CANVAS_MAX_W) ? CANVAS_MAX_W : tile_w[slot];
    int canvas_h = tile_h[slot] - 24;
    if (canvas_h > CANVAS_MAX_H) canvas_h = CANVAS_MAX_H;

    tiles[slot].ctx.id = slot;
    tiles[slot].ctx.width = canvas_w;
    tiles[slot].ctx.height = canvas_h;
    tiles[slot].ctx.buffer = tile_buffers[slot];

    uint32_t total_pixels = (uint32_t)canvas_w * (uint32_t)canvas_h;
    for (uint32_t p = 0; p < total_pixels; p++) tile_buffers[slot][p] = bg_color;

    tile_count++;
    active_tile_idx = slot;
}

static void wm_remove_tile(int idx) {
    if (idx < 0 || idx >= tile_count) return;
    uint32_t *removed_buf = tile_buffers[idx];
    for (int i = idx; i < tile_count - 1; i++) {
        tiles[i] = tiles[i + 1];
        tiles[i].id = i;
        tiles[i].ctx.id = i;
        tile_buffers[i] = tile_buffers[i + 1];
        tile_x[i] = tile_x[i + 1];
        tile_y[i] = tile_y[i + 1];
        tile_w[i] = tile_w[i + 1];
        tile_h[i] = tile_h[i + 1];
        saved_x[i] = saved_x[i + 1];
        saved_y[i] = saved_y[i + 1];
        saved_w[i] = saved_w[i + 1];
        saved_h[i] = saved_h[i + 1];
        is_maximized[i] = is_maximized[i + 1];
    }
    tile_buffers[tile_count - 1] = removed_buf;
    tile_count--;

    if (drag_tile_idx == idx) { drag_active = 0; drag_tile_idx = -1; }
    else if (drag_tile_idx > idx) drag_tile_idx--;

    if (active_tile_idx == idx) active_tile_idx = (tile_count > 0) ? tile_count - 1 : -1;
    else if (active_tile_idx > idx) active_tile_idx--;
}

win_context_t thiswindowname(const char *title) {
    for (int i = 0; i < tile_count; i++) {
        if (kstrcmp(tiles[i].title, title) == 0) return tiles[i].ctx;
    }
    win_context_t empty = { .id = -1 };
    return empty;
}

static void wm_toggle_maximize(int idx) {
    if (idx < 0 || idx >= tile_count) return;

    int screen_w = (int)fb_get_width();
    int screen_h = (int)fb_get_height();

    if (!is_maximized[idx]) {
        saved_x[idx] = tile_x[idx];
        saved_y[idx] = tile_y[idx];
        saved_w[idx] = tile_w[idx];
        saved_h[idx] = tile_h[idx];

        tile_x[idx] = 0;
        tile_y[idx] = TOP_BAR_H;
        tile_w[idx] = screen_w;
        tile_h[idx] = screen_h - TOP_BAR_H;
        is_maximized[idx] = 1;
    } else {
        tile_x[idx] = saved_x[idx];
        tile_y[idx] = saved_y[idx];
        tile_w[idx] = saved_w[idx];
        tile_h[idx] = saved_h[idx];
        is_maximized[idx] = 0;
    }

    int canvas_w = (tile_w[idx] > CANVAS_MAX_W) ? CANVAS_MAX_W : tile_w[idx];
    int canvas_h = tile_h[idx] - 24;
    if (canvas_h > CANVAS_MAX_H) canvas_h = CANVAS_MAX_H;

    tiles[idx].ctx.width = canvas_w;
    tiles[idx].ctx.height = canvas_h;

    if (tiles[idx].ctx.on_event) {
        wm_event_t evt;
        evt.type = WM_EVENT_REDRAW;
        evt.param1 = 0;
        evt.param2 = 0;
        tiles[idx].ctx.on_event(&tiles[idx].ctx, &evt);
    }
}

static void wm_draw_top_bar(void) {
    int screen_w = fb_get_width();
    draw_rect(0, 0, screen_w, TOP_BAR_H, 0x00111111);
    draw_rect(0, TOP_BAR_H, screen_w, 1, 0x00333333);
    fb_draw_string(14, 22, "Atom32", COLOR_WHITE, 0x00111111);

    int btn_x = 110, btn_w = 95, btn_h = 24, gap = 8;
    int count = get_app_count();

    for (int i = 0; i < count; i++) {
        const app_descriptor_t *app = get_app_by_index(i);
        if (!app) continue;

        ui_button_t btn;
        ui_button_init(&btn, btn_x + i * (btn_w + gap), 4, btn_w, btn_h, app->name);
        btn.bg_color = 0x00333333; 
        btn.border_color = 0x00555555;
        ui_button_draw(&btn);
    }

    ui_button_t sys_btn;
    ui_button_init(&sys_btn, screen_w - 105, 4, 95, 24, "info");
    sys_btn.bg_color = 0x00222222;
    sys_btn.border_color = COLOR_BLUE;
    ui_button_draw(&sys_btn);
}

static void wm_draw_system_info_modal(void) {
    int screen_w = fb_get_width();
    int screen_h = fb_get_height();

    int modal_w = 360;
    int modal_h = 230;
    int modal_x = (screen_w - modal_w) / 2;
    int modal_y = (screen_h - modal_h) / 2;

    draw_rect(modal_x - 3, modal_y - 3, modal_w + 6, modal_h + 6, COLOR_THINKPAD_RED);
    draw_rect(modal_x, modal_y, modal_w, modal_h, 0x00181818);

    draw_rect(modal_x, modal_y, modal_w, 32, 0x00282828);
    fb_draw_string(modal_x + 15, modal_y + 22, "System Information", COLOR_WHITE, 0x00282828);

    int ty = modal_y + 55;
    fb_draw_string(modal_x + 20, ty,       "Product Name:    Atom32 ", 0x00CCCCCC, 0x00181818);
    fb_draw_string(modal_x + 20, ty + 22,  "Product Version: 0.5.0", 0x00CCCCCC, 0x00181818);
    fb_draw_string(modal_x + 20, ty + 44,  "Product Build: DH26YU9K12", 0x00CCCCCC, 0x00181818);
    fb_draw_string(modal_x + 20, ty + 66,  "Version Date:  0.5 14.09.2026", 0x00CCCCCC, 0x00181818);
    fb_draw_string(modal_x + 20, ty + 88,  "More info. atomglide.com/atom32", 0x00CCCCCC, 0x00181818);

    ui_button_t close_btn;
    ui_button_init(&close_btn, modal_x + (modal_w - 90) / 2, modal_y + modal_h - 38, 90, 26, "Close");
    close_btn.bg_color = 0x00333333;
    close_btn.border_color = 0x00666666;
    ui_button_draw(&close_btn);
}

void wm_draw_all(void) {
    wm_draw_top_bar();
    
    for (int i = 0; i < tile_count; i++) {
        int x, y, w, h;
        get_tile_bounds(i, tile_count, &x, &y, &w, &h);

        uint32_t border_color = (i == active_tile_idx) ? COLOR_THINKPAD_RED : COLOR_BLACK;
        draw_rect(x - 2, y - 2, w + 4, h + 4, border_color);

        uint32_t header_color = (i == active_tile_idx) ? 0x00222222 : 0x00444444;
        draw_rect(x, y, w, 24, header_color);
        fb_draw_string(x + 10, y + 18, tiles[i].title, COLOR_WHITE, header_color);

        int max_circle_x = x + w - 32;
        int circle_y = y + 12;
        int circle_radius = 6;
        draw_filled_circle(max_circle_x, circle_y, circle_radius + 1, 0x00005500);
        draw_filled_circle(max_circle_x, circle_y, circle_radius, 0x0000AA00);

        int close_circle_x = x + w - 14;
        draw_filled_circle(close_circle_x, circle_y, circle_radius + 1, 0x00550000);
        draw_filled_circle(close_circle_x, circle_y, circle_radius, COLOR_BTN_CLOSE);

        draw_rect(x, y + 24, w, h - 24, tiles[i].bg_color);

        win_context_t *ctx = &tiles[i].ctx;
        for (int cy = 0; cy < ctx->height; cy++) {
            for (int cx = 0; cx < ctx->width; cx++) {
                uint32_t color = ctx->buffer[cy * ctx->width + cx];
                if (color != 0x00000000) fb_put_pixel(x + cx, y + 24 + cy, color);
            }
        }
    }

    if (drag_active && drag_tile_idx >= 0 && drag_tile_idx < tile_count) {
        int idx = drag_tile_idx;
        draw_wireframe_rect(wireframe_x, wireframe_y, tile_w[idx], tile_h[idx], COLOR_THINKPAD_RED);
    }

    if (show_system_info_modal) {
        wm_draw_system_info_modal();
    }
}

void wm_update_window(win_context_t *ctx) {
    if (ctx->id == -1 || show_system_info_modal) return;
    int x, y, w, h;
    get_tile_bounds(ctx->id, tile_count, &x, &y, &w, &h);
    for (int cy = 0; cy < ctx->height; cy++) {
        for (int cx = 0; cx < ctx->width; cx++) {
            uint32_t color = ctx->buffer[cy * ctx->width + cx];
            fb_put_pixel(x + cx, y + 24 + cy, color ? color : tiles[ctx->id].bg_color);
        }
    }
    fb_swap_rect(x, y + 24, ctx->width, ctx->height);
}

void wm_draw_char_rect(win_context_t *ctx, int char_x, int char_y) {
    if (ctx->id == -1 || show_system_info_modal) return;
    int win_x, win_y, win_w, win_h;
    get_tile_bounds(ctx->id, tile_count, &win_x, &win_y, &win_w, &win_h);

    int font_h = FreeSans9pt7b.yAdvance, char_w = 16;
    int start_canvas_y = char_y - font_h + 6;
    if (start_canvas_y < 0) start_canvas_y = 0;

    for (int cy = start_canvas_y; cy < start_canvas_y + font_h; cy++) {
        for (int cx = char_x; cx < char_x + char_w; cx++) {
            if (cx < ctx->width && cy < ctx->height) {
                uint32_t color = ctx->buffer[cy * ctx->width + cx];
                fb_put_pixel(win_x + cx, win_y + 24 + cy, color ? color : tiles[ctx->id].bg_color);
            }
        }
    }
    fb_swap_rect(win_x + char_x, win_y + 24 + start_canvas_y, char_w, font_h);
}

void wm_handle_mouse(int mouse_x, int mouse_y, uint8_t left_button) {
    if (!left_button) {
        if (drag_active && drag_tile_idx >= 0 && drag_tile_idx < tile_count) {
            tile_x[drag_tile_idx] = wireframe_x;
            tile_y[drag_tile_idx] = wireframe_y;
            drag_active = 0;
            drag_tile_idx = -1;

            fb_clear(0x00050505);
            wm_draw_all();
            fb_swap_buffers();
        }
        return;
    }

    if (drag_active && drag_tile_idx >= 0 && drag_tile_idx < tile_count) {
        int idx = drag_tile_idx;
        int screen_w = (int)fb_get_width();
        int screen_h = (int)fb_get_height();

        int new_x = mouse_x - drag_offset_x;
        int new_y = mouse_y - drag_offset_y;

        if (new_y < TOP_BAR_H) new_y = TOP_BAR_H;
        if (new_x < -(tile_w[idx] - 40)) new_x = -(tile_w[idx] - 40);
        if (new_x > screen_w - 40) new_x = screen_w - 40;
        if (new_y > screen_h - 24) new_y = screen_h - 24;

        wireframe_x = new_x;
        wireframe_y = new_y;

        fb_clear(0x00050505);
        wm_draw_all();
        fb_swap_buffers();
        return;
    }

    if (show_system_info_modal) {
        int screen_w = fb_get_width();
        int screen_h = fb_get_height();
        int modal_w = 360;
        int modal_h = 230;
        int modal_x = (screen_w - modal_w) / 2;
        int modal_y = (screen_h - modal_h) / 2;

        int close_btn_x = modal_x + (modal_w - 90) / 2;
        int close_btn_y = modal_y + modal_h - 38;

        if (mouse_x >= close_btn_x && mouse_x <= close_btn_x + 90 &&
            mouse_y >= close_btn_y && mouse_y <= close_btn_y + 26) {
            show_system_info_modal = 0;
            fb_clear(0x00050505);
            wm_draw_all();
            fb_swap_buffers();
        }
        return;
    }

    if (mouse_y < TOP_BAR_H) {
        int screen_w = fb_get_width();

        if (mouse_x >= screen_w - 105 && mouse_x <= screen_w - 10) {
            show_system_info_modal = 1;
            wm_draw_all();
            fb_swap_buffers();
            return;
        }

        int btn_x = 110, btn_w = 95, gap = 8;
        int count = get_app_count();

        for (int i = 0; i < count; i++) {
            const app_descriptor_t *app = get_app_by_index(i);
            if (!app) continue;

            int x = btn_x + i * (btn_w + gap);
            if (mouse_x >= x && mouse_x < x + btn_w) {
                wm_add_tile(app->name, app->bg_color, app->handler);
                win_context_t ctx = thiswindowname(app->name);
                if (ctx.id != -1 && app->init) {
                    app->init(&ctx);
                }
                break;
            }
        }
        fb_clear(0x00050505);
        wm_draw_all();
        fb_swap_buffers();
        return;
    }

    for (int i = tile_count - 1; i >= 0; i--) {
        int x, y, w, h;
        get_tile_bounds(i, tile_count, &x, &y, &w, &h);

        int inside = (mouse_x >= x - 2 && mouse_x < x + w + 2 &&
                      mouse_y >= y - 2 && mouse_y < y + h + 2);
        if (!inside) continue;

        if (mouse_y >= y && mouse_y < y + 24) {
            int close_x = x + w - 14;
            int max_x = x + w - 32;
            int circle_y = y + 12;
            int radius = 8;

            if ((mouse_x - close_x) * (mouse_x - close_x) + (mouse_y - circle_y) * (mouse_y - circle_y) <= radius * radius) {
                wm_remove_tile(i);
                fb_clear(0x00050505);
                wm_draw_all();
                fb_swap_buffers();
                return;
            }

            if ((mouse_x - max_x) * (mouse_x - max_x) + (mouse_y - circle_y) * (mouse_y - circle_y) <= radius * radius) {
                wm_bring_to_front(i);
                wm_toggle_maximize(tile_count - 1);
                fb_clear(0x00050505);
                wm_draw_all();
                fb_swap_buffers();
                return;
            }

            wm_bring_to_front(i);
            int idx = tile_count - 1;
            
            if (!is_maximized[idx]) {
                drag_active = 1;
                drag_tile_idx = idx;
                drag_offset_x = mouse_x - tile_x[idx];
                drag_offset_y = mouse_y - tile_y[idx];
                wireframe_x = tile_x[idx];
                wireframe_y = tile_y[idx];
            }

            fb_clear(0x00050505);
            wm_draw_all();
            fb_swap_buffers();
            return;
        }

        if (mouse_x >= x && mouse_x < x + w && mouse_y >= y + 24 && mouse_y < y + h) {
            if (i != tile_count - 1) {
                wm_bring_to_front(i);
                i = tile_count - 1;
                get_tile_bounds(i, tile_count, &x, &y, &w, &h);
            }
            if (active_tile_idx != i) {
                active_tile_idx = i;
                wm_draw_all();
                fb_swap_buffers();
            }

            if (tiles[i].ctx.on_event) {
                wm_event_t evt;
                evt.type = WM_EVENT_MOUSE;
                evt.param1 = (uint32_t)(mouse_x - x);
                evt.param2 = (uint32_t)(mouse_y - (y + 24));
                tiles[i].ctx.on_event(&tiles[i].ctx, &evt);
            }
            return;
        }
        return;
    }
}

void wm_dispatch_key(char key) {
    if (show_system_info_modal) return;
    if (tile_count == 0 || active_tile_idx == -1) return;
    win_context_t *active_ctx = &tiles[active_tile_idx].ctx;
    if (active_ctx->on_event != 0) {
        wm_event_t evt = {WM_EVENT_KEY, (uint32_t)key, 0};
        active_ctx->on_event(active_ctx, &evt);
    }
}