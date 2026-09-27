// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------



#include "klib.h"

uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void outb(uint16_t port, uint8_t data) {
    __asm__ volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}

void sleep_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms; i++) {
        for (volatile int j = 0; j < 50000; j++) {
            __asm__ volatile ("nop");
        }
    }
}

int kstrlen(const char *str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

void kstrcpy(char *dest, const char *src) {
    while ((*dest++ = *src++));
}

void kstrcat(char *dest, const char *src) {
    while (*dest) dest++;
    while ((*dest++ = *src++));
}

int kstrcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

void itoa(int num, char *str) {
    int i = 0, is_negative = 0;
    if (num == 0) { str[i++] = '0'; str[i] = '\0'; return; }
    if (num < 0) { is_negative = 1; num = -num; }

    while (num != 0) {
        str[i++] = (num % 10) + '0';
        num /= 10;
    }
    if (is_negative) str[i++] = '-';
    str[i] = '\0';

    for (int start = 0, end = i - 1; start < end; start++, end--) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
    }
}

void speed_phi_str(int s, int t, char *out_buf) {
    if (t == 0) { kstrcpy(out_buf, "Err: Div 0"); return; }

    int integer = s / t;
    int rem = s % t;
    char temp[16];
    
    itoa(integer, out_buf);
    kstrcat(out_buf, ".");

    for (int i = 0; i < 3; i++) {
        rem *= 10;
        itoa(rem / t, temp);
        kstrcat(out_buf, temp);
        rem %= t;
    }
}

void sound_on(uint32_t frequency) {
    if (frequency == 0) return;

    uint32_t divisor = 1193180 / frequency;

    outb(0x43, 0xB6);
    outb(0x42, (uint8_t)(divisor & 0xFF));
    outb(0x42, (uint8_t)((divisor >> 8) & 0xFF));

    uint8_t current_state = inb(0x61);
    if ((current_state & 3) != 3) {
        outb(0x61, current_state | 3);
    }
}

void sound_off(void) {
    uint8_t current_state = inb(0x61) & 0xFC;
    outb(0x61, current_state);
}

void beep(uint32_t frequency, uint32_t duration_ms) {
    sound_on(frequency);
    sleep_ms(duration_ms);
    sound_off();
}



static volatile uint32_t sound_ticks_left = 0;
static volatile uint8_t  sound_is_on = 0;

void sound_start(uint32_t frequency, uint32_t duration_ticks) {
    if (frequency == 0) {
        sound_off();
        sound_is_on = 0;
        sound_ticks_left = 0;
        return;
    }
    sound_on(frequency);
    sound_ticks_left = duration_ticks;
    sound_is_on = 1;
}

void sound_stop(void) {
    sound_off();
    sound_is_on = 0;
    sound_ticks_left = 0;
}

void sound_service_tick(void) {
    if (!sound_is_on) return;
    if (sound_ticks_left == 0) {
        sound_off();
        sound_is_on = 0;
    } else {
        sound_ticks_left--;
    }
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}