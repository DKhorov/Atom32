// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------
// COMM. RUSSIA 



#include "driver/idt/idt.h"

extern void isr_default_wrapper(void);
extern void isr_irq1_wrapper(void);
extern void isr_irq12_wrapper(void);

static idt_entry_t idt[256];
static idt_ptr_t   idtp;

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__ ("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline void io_wait(void) { outb(0x80, 0); }

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low  = (base & 0xFFFF);
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].sel       = sel;
    idt[num].always0   = 0;
    idt[num].flags     = flags;
}

static void pic_remap(void) {
    outb(0x20, 0x11); io_wait();
    outb(0xA0, 0x11); io_wait();

    outb(0x21, 0x20); io_wait(); 
    outb(0xA1, 0x28); io_wait(); 

    outb(0x21, 0x04); io_wait();
    outb(0xA1, 0x02); io_wait();

    outb(0x21, 0x01); io_wait();
    outb(0xA1, 0x01); io_wait();

    // РАЗРЕШАЕМ: IRQ0 (Таймер), IRQ1 (Клава), IRQ2 (Каскад для мыши)
    outb(0x21, 0xF8);
    
    // РАЗРЕШАЕМ: IRQ12 (Мышь PS/2, это бит 4 на ведомом контроллере)
    outb(0xA1, 0xEF); 
}

void idt_init(void) {
    idtp.limit = (sizeof(idt_entry_t) * 256) - 1;
    idtp.base  = (uint32_t)&idt;

    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, (uint32_t)isr_default_wrapper, 0x10, 0x8E);
    }

    idt_set_gate(33, (uint32_t)isr_irq1_wrapper, 0x10, 0x8E);  // INT 33 = IRQ 1
    idt_set_gate(44, (uint32_t)isr_irq12_wrapper, 0x10, 0x8E); // INT 44 = IRQ 12

    pic_remap();
    __asm__ __volatile__("lidt %0" : : "m"(idtp));
}