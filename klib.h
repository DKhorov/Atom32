// --------------------------------------------------------
// Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
// AtomGlide Labs Production 2026
// Author and Developer: Dmitry Khorov (GitHub @dkhorov)
// Product ID: 4569-2194-0912-7899-3124 KLP
// Product Name: AtomGlide Atom32 OS
// About: atomglide.com/atom32
// Tools and Drivers: atomglide.com/doghouse
// --------------------------------------------------------



#ifndef KLIB_H
#define KLIB_H

#include <stdint.h>

uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t data);

int kstrlen(const char *str);
void kstrcpy(char *dest, const char *src);
void kstrcat(char *dest, const char *src);
int kstrcmp(const char *s1, const char *s2);

void itoa(int num, char *str);
void sound_start(uint32_t frequency, uint32_t duration_ticks);
void sound_stop(void);
void sound_service_tick(void);
void speed_phi_str(int s, int t, char *out_buf);
void sleep_ms(uint32_t ms);
void sound_on(uint32_t frequency);
void sound_off(void);
void beep(uint32_t frequency, uint32_t duration_ms);
int strcmp(const char *s1, const char *s2);

#endif