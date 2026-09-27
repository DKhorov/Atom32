// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------




#ifndef TASK_H
#define TASK_H

#include <stdint.h>

#define MAX_TASKS 16
#define STACK_SIZE 8192

typedef enum {
    TASK_READY,
    TASK_RUNNING,
    TASK_SLEEPING,
    TASK_DEAD
} task_state_t;

typedef struct {
    int id;
    char name[32];
    task_state_t state;
    uint32_t esp;
    uint8_t stack[STACK_SIZE] __attribute__((aligned(16)));
    void (*entry_point)(void);
} task_t;

void task_manager_init(void);
int task_create(const char *name, void (*entry_point)(void));
void task_yield(void);
task_t *task_get_current(void);

#endif