# --------------------------------------------------------
# Atom32 - 32bit Operation System. Bare-Metal Core on C,ASM
# AtomGlide Labs Production 2026
# Author and Developer: Dmitry Khorov (GitHub @dkhorov)
# Product ID: 4569-2194-0912-7899-3124 KLP
# Product Name: AtomGlide Atom32 OS
# About: atomglide.com/atom32
# Tools and Drivers: atomglide.com/doghouse
# --------------------------------------------------------
# ASM FILE




.set ALIGN,    1<<0             
.set MEMINFO,  1<<1            
.set VIDMODE,  1<<2            
.set FLAGS,    ALIGN | MEMINFO | VIDMODE
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.long 0, 0, 0, 0, 0

.long 0                        
.long 1366                      
.long 768                       
.long 32                        

.section .text
.global _start
.global fpu_init
.type _start, @function

fpu_init:
    mov %cr0, %eax
    and $0xFFFFFFFD, %eax       
    or  $0x00000002, %eax       
    mov %eax, %cr0

    mov %cr4, %eax
    or  $0x00000600, %eax      
    mov %eax, %cr4

    finit
    ret

_start:
    mov $stack_top, %esp

    push %ebx                   
    push %eax                 

    call kernel_main

    cli
1:  hlt
    jmp 1b

.section .bss
.align 16
stack_bottom:
.skip 16384                     
stack_top: