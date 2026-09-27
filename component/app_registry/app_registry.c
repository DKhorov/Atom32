// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------


#include "app_registry.h"
#include "component/graphics_system/colors.h"
#include "program/pong/pong.h"
#include "program/test/test.h"
#include "program/blaster/blaster.h"


extern void term_init(win_context_t *ctx);
extern void term_event_handler(win_context_t *ctx, wm_event_t *evt);

extern void doghouse_init(win_context_t *ctx);
extern void doghouse_event_handler(win_context_t *ctx, wm_event_t *evt);

extern void sysmon_draw(win_context_t *ctx);
extern void sysmon_event_handler(win_context_t *ctx, wm_event_t *evt);

extern void calc_init(win_context_t *ctx);
extern void calc_event_handler(win_context_t *ctx, wm_event_t *evt);

extern void test_app_init(win_context_t *ctx);
extern void test_app_event_handler(win_context_t *ctx, wm_event_t *evt);

extern void blaster_init(win_context_t *ctx);
extern void blaster_event_handler(win_context_t *ctx, wm_event_t *evt);

static app_descriptor_t app_table[MAX_REGISTERED_APPS];
static int app_count = 0;

static int kstrcasecmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        char c1 = *s1, c2 = *s2;
        if (c1 >= 'A' && c1 <= 'Z') c1 += 32;
        if (c2 >= 'A' && c2 <= 'Z') c2 += 32;
        if (c1 != c2) return c1 - c2;
        s1++; s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

int register_app(const char *name, uint32_t bg_color, void (*init)(win_context_t*), win_event_handler_t handler) {
    if (app_count >= MAX_REGISTERED_APPS) return 0;

    app_table[app_count].name = name;
    app_table[app_count].bg_color = bg_color;
    app_table[app_count].init = init;
    app_table[app_count].handler = handler;
    app_count++;
    
    return 1;
}

void app_registry_init(void) {
    app_count = 0;
    
    register_app("Atom Shell", COLOR_BLACK,  term_init,     term_event_handler);
    register_app("File Editor", COLOR_BLACK,  doghouse_init,  doghouse_event_handler);
    register_app("System Log",   0x000F172A,   sysmon_draw,    sysmon_event_handler);
    register_app("mathCalc",     0x00111111,   calc_init,      calc_event_handler);
    register_app("Pong", COLOR_BLACK, pong_init, pong_event_handler);
    register_app("Test", COLOR_BLACK, pong_init, test_app_event_handler);
    register_app("Sound Blaster", COLOR_BLACK, pong_init, blaster_event_handler);

}

const app_descriptor_t* get_app_by_index(int index) {
    if (index < 0 || index >= app_count) return 0;
    return &app_table[index];
}

int get_app_count(void) {
    return app_count;
}

const app_descriptor_t* find_app_by_name(const char *name) {
    for (int i = 0; i < app_count; i++) {
        if (kstrcasecmp(app_table[i].name, name) == 0) {
            return &app_table[i];
        }
    }
    return 0;
}