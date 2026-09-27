// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------




#include "component/graphics_system/grap.h"
#include "component/alert_system/alert.h"
#include "driver/trakpoint_sys/point.h"
#include "component/windows_system/win.h"
#include "component/graphics_system/colors.h"
#include "component/app_registry/app_registry.h"
#include "program/terminal/terminal.h"
#include "program/sysmon/sysmon.h"
#include "component/error/error.h"
#include "component/screensaver/saver.h"
#include "component/task_manager/task.h"
#include "driver/idt/idt.h"
#include "component/memory/paging.h"
#include "program/pong/pong.h"
#include "program/test/test.h"
#include "program/blaster/blaster.h"
#include "klib.h"

extern void fpu_init(void);
extern char get_keyboard_key(void);

void task_sysmon_entry(void) {
    uint32_t tick_counter = 0;
    while (1) {
        tick_counter++;
        if (tick_counter % 50000 == 0) {
            sysmon_print("Task SysMon Tick...");
        }
        task_yield();
    }
}

void task_pong_entry(void) {
    while (1) {
        pong_tick();
        task_yield();
    }
}


void task_blaster_entry(void) {
    while (1) {
        blaster_tick();
        task_yield();
    }
}


void task_sound_entry(void) {
    while (1) {
        sound_service_tick();
        for (volatile int i = 0; i < 50000; i++) { __asm__ volatile ("nop"); }
        task_yield();
    }
}

void task_input_gui_entry(void) {
    int last_x = -1;
    int last_y = -1;
    uint8_t last_btn = 0;

    while (1) {
        int key_pressed = 0;
        char key;
        while ((key = get_keyboard_key()) != '\0') {
            wm_dispatch_key(key);
            key_pressed = 1;
        }

        int cur_x = g_tp.x;
        int cur_y = g_tp.y;
        uint8_t cur_btn = g_tp.left_button;

        int mouse_moved = (cur_x != last_x || cur_y != last_y);
        int button_changed = (cur_btn != last_btn);

        
        int dragging_frame = (cur_btn && mouse_moved);

        if (button_changed || dragging_frame) {
            wm_handle_mouse(cur_x, cur_y, cur_btn);
        }

        if (button_changed || key_pressed || dragging_frame) {
            fb_clear(0x00050505);
            wm_draw_all();
            fb_swap_buffers();
        } else if (mouse_moved && last_x != -1) {
            int erase_x = (last_x - 2 < 0) ? 0 : last_x - 2;
            int erase_y = (last_y - 2 < 0) ? 0 : last_y - 2;
            fb_swap_rect(erase_x, erase_y, 24, 24);
        }

        trackpoint_draw_direct();

        last_x = cur_x;
        last_y = cur_y;
        last_btn = cur_btn;

        task_yield();
    }
}

void kernel_main(uint32_t magic, multiboot_info_t *mb_info) {
    if (magic != 0x2BADB002) return;

    __asm__ __volatile__("cli");

    fpu_init();
    fb_init(mb_info);
    syslog_init();

    uint32_t lfb_addr = 0;
    if (mb_info && (mb_info->flags & (1 << 12))) {
        lfb_addr = mb_info->framebuffer_addr;
    }
    if (lfb_addr == 0) {
        lfb_addr = 0xFD000000;
    }

    paging_init(lfb_addr);
    idt_init();

    app_registry_init();

    syslog_info("AtomGlide Labs Production 2026.");
    syslog_info("Read: https://atomglide.com/atom32 ");
    syslog_info("Setup user interface...");

    task_manager_init();

    fb_swap_buffers();

    for (volatile int i = 0; i < 90000000; i++) { __asm__("nop"); }

    saver_show_boot(1000);

    wm_init();
    
    wm_add_tile("Atom Shell", COLOR_BLACK, term_event_handler);
    sysmon_print("Shell is starting...");
    win_context_t term_ctx = thiswindowname("Atom Shell");
    if (term_ctx.id != -1) {
        term_init(&term_ctx);
    }

    wm_add_tile("TEST", COLOR_BLACK, test_app_event_handler);
win_context_t test_ctx = thiswindowname("TEST");
if (test_ctx.id != -1) {
    test_app_init(&test_ctx);
}

    trackpoint_init();

    fb_clear(0x00050505);
    wm_draw_all();
    fb_swap_buffers();
    trackpoint_draw_direct();

    task_create("GUI Input Loop", task_input_gui_entry);
    task_create("SysMon Worker", task_sysmon_entry);
    task_create("Pong Worker", task_pong_entry);
    task_create("Blaster Worker", task_blaster_entry);
    task_create("Sound Worker", task_sound_entry);

    sysmon_print("Multitasking Engine Online");

    __asm__ __volatile__("sti");


    while (1) {
        task_yield();
    }
}