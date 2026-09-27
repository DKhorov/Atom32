# --------------------------------------------------------
# Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
# AtomGlide Labs Production 2026
# Author and Developer: Dmitry Khorov (GitHub @dkhorov)
# Product ID: 4569-2194-0912-7899-3124 KLP
# Product Name: AtomGlide Atom32 OS
# About: atomglide.com/atom32
# Tools and Drivers: atomglide.com/doghouse
# --------------------------------------------------------
# DONT IN GITHUB! Please enter this file in ignor




export PATH := /opt/homebrew/bin:/usr/local/bin:$(PATH)

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Darwin)
    CC = x86_64-elf-gcc
    AS = x86_64-elf-as
    LD = x86_64-elf-ld
    GRUB_MKRESCUE = i686-elf-grub-mkrescue
else
    CC = gcc
    AS = as
    LD = ld
    GRUB_MKRESCUE = grub-mkrescue
endif

CFLAGS = -m32 -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I.
ASFLAGS = --32
LDFLAGS = -m elf_i386 -T linker.ld

C_SOURCES = $(shell find . -type f -name '*.c')
ASM_SOURCES = $(shell find . -type f -name '*.s')

OBJS = $(ASM_SOURCES:.s=.o) $(C_SOURCES:.c=.o)

all: atomglide.iso

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

myos.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

atomglide.iso: myos.bin
	mkdir -p isodir/boot/grub
	cp myos.bin isodir/boot/atomglide.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	$(GRUB_MKRESCUE) -o atomglide.iso isodir

clean:
	find . -type f -name '*.o' -delete
	rm -f myos.bin atomglide.bin atomglide.iso
	rm -rf isodir

.PHONY: all clean