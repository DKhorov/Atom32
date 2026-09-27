// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------

#ifndef GRAP_H
#define GRAP_H

#include <stdint.h>
#include "font.h"
#include "component/windows_system/win.h"

typedef struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
    uint8_t  color_info[6];
} multiboot_info_t;

void fb_init(multiboot_info_t *mb_info);
void fb_clear(uint32_t color);
void fb_swap_buffers(void);
void fb_swap_rect(int x, int y, int width, int height);

uint32_t fb_get_width(void);
uint32_t fb_get_height(void);

void fb_put_pixel(uint32_t x, uint32_t y, uint32_t color);
void fb_put_pixel_direct(int x, int y, uint32_t color);
uint32_t fb_get_pixel(uint32_t x, uint32_t y);
void draw_rect(int x, int y, int width, int height, uint32_t color);
void draw_rect_canvas(win_context_t *ctx, int x, int y, int w, int h, uint32_t color);
int fb_draw_char(int x, int y, char c, uint32_t fg_color, uint32_t bg_color);
void fb_draw_string(int x, int y, const char *str, uint32_t fg_color, uint32_t bg_color);
int draw_char_canvas(win_context_t *ctx, int x, int y, char c, uint32_t fg);
void draw_string_canvas(win_context_t *ctx, int x, int y, const char *str, uint32_t color);

#endif