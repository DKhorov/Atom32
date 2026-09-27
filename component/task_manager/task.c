// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------






#include "task.h"
#include "component/error/error.h"

extern void switch_context(uint32_t *old_esp, uint32_t new_esp);

static task_t tasks[MAX_TASKS];
static int current_task_id = -1;
static int total_tasks = 0;

void task_manager_init(void) {
    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].id = -1;
        tasks[i].state = TASK_DEAD;
    }

    tasks[0].id = 0;
    tasks[0].state = TASK_RUNNING;
    tasks[0].entry_point = 0;
    
    const char *kname = "Kernel Main";
    int idx = 0;
    while (kname[idx] && idx < 31) {
        tasks[0].name[idx] = kname[idx];
        idx++;
    }
    tasks[0].name[idx] = '\0';

    current_task_id = 0;
    total_tasks = 1;
}

int task_create(const char *name, void (*entry_point)(void)) {
    if (total_tasks >= MAX_TASKS || !entry_point) {
        error("TASK_CREATE: Limit reached or NULL entry point");
        return -1;
    }

    int slot = -1;
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].state == TASK_DEAD) {
            slot = i;
            break;
        }
    }

    if (slot == -1) {
        error("TASK_CREATE: No free process slots");
        return -1;
    }

    task_t *t = &tasks[slot];
    t->id = slot;
    t->state = TASK_READY;
    t->entry_point = entry_point;

    int idx = 0;
    while (name[idx] && idx < 31) {
        t->name[idx] = name[idx];
        idx++;
    }
    t->name[idx] = '\0';

    // Подготовка фрейма стека
    uint32_t *sp = (uint32_t *)&t->stack[STACK_SIZE - 4];

    *sp-- = (uint32_t)entry_point; 
    *sp-- = 0x0202;               
    
    *sp-- = 0;                     // EAX
    *sp-- = 0;                     // ECX
    *sp-- = 0;                     // EDX
    *sp-- = 0;                     // EBX
    *sp-- = 0;                     // ESP (игнорируется popa)
    *sp-- = 0;                     // EBP
    *sp-- = 0;                     // ESI
    *sp   = 0;                     // EDI

    t->esp = (uint32_t)sp;
    total_tasks++;

    return slot;
}

void task_yield(void) {
    if (total_tasks <= 1) return;

    int next_task_id = (current_task_id + 1) % MAX_TASKS;

    int searched = 0;
    while (tasks[next_task_id].state != TASK_READY && tasks[next_task_id].state != TASK_RUNNING) {
        next_task_id = (next_task_id + 1) % MAX_TASKS;
        searched++;
        if (searched > MAX_TASKS) {
            error("SCHEDULER PANIC: No runnable tasks");
            return;
        }
    }

    if (next_task_id == current_task_id) return;

    int prev_id = current_task_id;
    current_task_id = next_task_id;

    if (tasks[prev_id].state == TASK_RUNNING) {
        tasks[prev_id].state = TASK_READY;
    }
    tasks[current_task_id].state = TASK_RUNNING;

    switch_context(&tasks[prev_id].esp, tasks[current_task_id].esp);
}

task_t *task_get_current(void) {
    if (current_task_id >= 0 && current_task_id < MAX_TASKS) {
        return &tasks[current_task_id];
    }
    return 0;
}