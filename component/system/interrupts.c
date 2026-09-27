// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------





#include <stdint.h>
#include "driver/trakpoint_sys/point.h"

#define KBD_BUF_SIZE 256
volatile char kbd_buffer[KBD_BUF_SIZE];
volatile int kbd_head = 0;
volatile int kbd_tail = 0;

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__ ("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static char scancode_to_char(uint8_t code) {
    if (code & 0x80) return '\0'; 
    switch (code) {
        case 0x02: return '1'; case 0x03: return '2'; case 0x04: return '3';
        case 0x05: return '4'; case 0x06: return '5'; case 0x07: return '6';
        case 0x08: return '7'; case 0x09: return '8'; case 0x0A: return '9';
        case 0x0B: return '0';
        case 0x1E: return 'a'; case 0x30: return 'b'; case 0x2E: return 'c';
        case 0x20: return 'd'; case 0x12: return 'e'; case 0x21: return 'f';
        case 0x22: return 'g'; case 0x23: return 'h'; case 0x17: return 'i';
        case 0x24: return 'j'; case 0x25: return 'k'; case 0x26: return 'l';
        case 0x32: return 'm'; case 0x31: return 'n'; case 0x18: return 'o';
        case 0x19: return 'p'; case 0x10: return 'q'; case 0x13: return 'r';
        case 0x1F: return 's'; case 0x14: return 't'; case 0x16: return 'u';
        case 0x2F: return 'v'; case 0x11: return 'w'; case 0x2D: return 'x';
        case 0x15: return 'y'; case 0x2C: return 'z';
        case 0x1C: return '\n'; case 0x0E: return '\b'; case 0x39: return ' ';
        default: return '\0';
    }
}

char get_keyboard_key(void) {
    if (kbd_head == kbd_tail) return '\0';
    char key = kbd_buffer[kbd_tail];
    kbd_tail = (kbd_tail + 1) % KBD_BUF_SIZE;
    return key;
}

void default_irq_handler(void) {
    outb(0x20, 0x20);
    outb(0xA0, 0x20);
}

void irq1_handler(void) {
    uint8_t status = inb(0x64);
    if (status & 0x01) {
        uint8_t data = inb(0x60);
        char key = scancode_to_char(data);
        if (key != '\0') {
            int next_head = (kbd_head + 1) % KBD_BUF_SIZE;
            if (next_head != kbd_tail) {
                kbd_buffer[kbd_head] = key;
                kbd_head = next_head;
            }
        }
    }
    outb(0x20, 0x20); 
}

void irq12_handler(void) {
    trackpoint_poll();
    outb(0xA0, 0x20);
    outb(0x20, 0x20);
}