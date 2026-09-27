// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------
// WinBSI Driver. Framebuffer graphics


#include "component/graphics_system/grap.h"
#include "component/graphics_system/font.h"
#include "component/windows_system/win.h"

static uint32_t *lfb_addr = 0;
static uint32_t screen_width = 0;
static uint32_t screen_height = 0;
static uint32_t screen_pitch = 0;

static uint32_t back_buffer[1920 * 1080];

static void *fast_memcpy(void *dest, const void *src, uint32_t count) {
    uint32_t *d = (uint32_t *)dest;
    const uint32_t *s = (const uint32_t *)src;
    uint32_t dwords = count / 4;
    while (dwords--) *d++ = *s++;
    return dest;
}

void fb_init(multiboot_info_t *mb_info) {
    if (!mb_info) return;
    lfb_addr = (uint32_t *)(uintptr_t)mb_info->framebuffer_addr;
    screen_width = mb_info->framebuffer_width;
    screen_height = mb_info->framebuffer_height;
    screen_pitch = mb_info->framebuffer_pitch;
}

uint32_t fb_get_width(void) { return screen_width; }
uint32_t fb_get_height(void) { return screen_height; }

void fb_put_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= screen_width || y >= screen_height) return;
    back_buffer[y * screen_width + x] = color;
}

void fb_put_pixel_direct(int x, int y, uint32_t color) {
    if (x < 0 || y < 0 || x >= (int)screen_width || y >= (int)screen_height || !lfb_addr) return;
    uint32_t pitch_in_pixels = screen_pitch / 4;
    lfb_addr[y * pitch_in_pixels + x] = color;
}

uint32_t fb_get_pixel(uint32_t x, uint32_t y) {
    if (x >= screen_width || y >= screen_height) return 0;
    return back_buffer[y * screen_width + x];
}

void fb_clear(uint32_t color) {
    uint32_t total = screen_width * screen_height;
    for (uint32_t i = 0; i < total; i++) back_buffer[i] = color;
}

void draw_rect(int x, int y, int width, int height, uint32_t color) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fb_put_pixel(x + j, y + i, color);
        }
    }
}

int fb_draw_char(int x, int y, char c, uint32_t fg_color, uint32_t bg_color) {
    uint8_t uc = (uint8_t)c;
    const GFXfont *font = &FreeSans9pt7b;

    if (uc < font->first || uc > font->last) return 0;

    GFXglyph *glyph = &font->glyph[uc - font->first];
    uint8_t *bitmap = font->bitmap;

    uint16_t bo = glyph->bitmapOffset;
    uint8_t  w  = glyph->width;
    uint8_t  h  = glyph->height;
    int8_t   xo = glyph->xOffset;
    int8_t   yo = glyph->yOffset;

    uint8_t  bit = 0;
    uint16_t bits = 0;

    if (bg_color != 0xFFFFFFFF) {
        draw_rect(x, y - font->yAdvance + 6, glyph->xAdvance, font->yAdvance, bg_color);
    }

    for (int yy = 0; yy < h; yy++) {
        for (int xx = 0; xx < w; xx++) {
            if (!(bit++ & 7)) {
                bits = bitmap[bo++];
            }
            if (bits & 0x80) {
                fb_put_pixel(x + xo + xx, y + yo + yy, fg_color);
            }
            bits <<= 1;
        }
    }

    return glyph->xAdvance;
}

void fb_draw_string(int x, int y, const char *str, uint32_t fg_color, uint32_t bg_color) {
    int cur_x = x;
    int cur_y = y;
    const GFXfont *font = &FreeSans9pt7b;

    while (*str) {
        if (*str == '\n') {
            cur_x = x;
            cur_y += font->yAdvance;
            str++;
            continue;
        }
        if (*str == '\r') {
            cur_x = x;
            str++;
            continue;
        }
        if (*str == '\t') {
            cur_x += font->glyph[0].xAdvance * 4;
            str++;
            continue;
        }

        int advance = fb_draw_char(cur_x, cur_y, *str, fg_color, bg_color);
        cur_x += advance;
        str++;
    }
}

void fb_swap_buffers(void) {
    if (!lfb_addr || screen_width == 0) return;
    uint32_t pitch_in_pixels = screen_pitch / 4;
    uint32_t line_bytes = screen_width * 4;
    for (uint32_t y = 0; y < screen_height; y++) {
        fast_memcpy(&lfb_addr[y * pitch_in_pixels], &back_buffer[y * screen_width], line_bytes);
    }
}

void fb_swap_rect(int x, int y, int width, int height) {
    if (!lfb_addr || screen_width == 0) return;

    if (x < 0) { width += x; x = 0; }
    if (y < 0) { height += y; y = 0; }
    if (x + width > (int)screen_width) width = screen_width - x;
    if (y + height > (int)screen_height) height = screen_height - y;
    if (width <= 0 || height <= 0) return;

    uint32_t pitch_in_pixels = screen_pitch / 4;
    uint32_t copy_bytes = width * 4;

    for (int cy = y; cy < y + height; cy++) {
        uint32_t *src = &back_buffer[cy * screen_width + x];
        uint32_t *dst = &lfb_addr[cy * pitch_in_pixels + x];
        fast_memcpy(dst, src, copy_bytes);
    }
}


int draw_char_canvas(win_context_t *ctx, int x, int y, char c, uint32_t fg) {
    uint8_t uc = (uint8_t)c;
    const GFXfont *font = &FreeSans9pt7b;

    if (uc < font->first || uc > font->last) return 8;

    GFXglyph *glyph = &font->glyph[uc - font->first];
    uint8_t *bitmap = font->bitmap;

    uint16_t bo = glyph->bitmapOffset;
    uint8_t  w  = glyph->width;
    uint8_t  h  = glyph->height;
    int8_t   xo = glyph->xOffset;
    int8_t   yo = glyph->yOffset;

    uint8_t  bit = 0;
    uint16_t bits = 0;

    for (int yy = 0; yy < h; yy++) {
        for (int xx = 0; xx < w; xx++) {
            if (!(bit++ & 7)) {
                bits = bitmap[bo++];
            }
            if (bits & 0x80) {
                int px = x + xo + xx;
                int py = y + yo + yy;
                if (px >= 0 && px < ctx->width && py >= 0 && py < ctx->height) {
                    ctx->buffer[py * ctx->width + px] = fg;
                }
            }
            bits <<= 1;
        }
    }

    return glyph->xAdvance;
}

void draw_string_canvas(win_context_t *ctx, int x, int y, const char *str, uint32_t color) {
    int cur_x = x;
    while (*str) {
        int adv = draw_char_canvas(ctx, cur_x, y, *str, color);
        cur_x += (adv > 0) ? adv : 8;
        str++;
    }
}

void draw_rect_canvas(win_context_t *ctx, int x, int y, int w, int h, uint32_t color) {
    for (int py = y; py < y + h; py++) {
        if (py < 0 || py >= ctx->height) continue;
        for (int px = x; px < x + w; px++) {
            if (px < 0 || px >= ctx->width) continue;
            ctx->buffer[py * ctx->width + px] = color;
        }
    }
}