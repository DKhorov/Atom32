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

.global switch_context
.type switch_context, @function

# void switch_context(uint32_t *old_esp, uint32_t new_esp)
switch_context:
    pushfl
    pusha

    movl 40(%esp), %eax
    movl %esp, (%eax)

    movl 44(%esp), %edx
    movl %edx, %esp

    popa
    popfl

    ret