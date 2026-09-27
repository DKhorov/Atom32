// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------

#ifndef APP_REGISTRY_H
#define APP_REGISTRY_H

#include "component/windows_system/win.h"

#define MAX_REGISTERED_APPS 16

typedef struct {
    const char *name;                 
    uint32_t bg_color;              
    void (*init)(win_context_t *ctx); 
    win_event_handler_t handler;     
} app_descriptor_t;

void app_registry_init(void);
int register_app(const char *name, uint32_t bg_color, void (*init)(win_context_t*), win_event_handler_t handler);

const app_descriptor_t* get_app_by_index(int index);
int get_app_count(void);
const app_descriptor_t* find_app_by_name(const char *name);

#endif