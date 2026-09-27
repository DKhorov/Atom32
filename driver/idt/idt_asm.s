.global isr_default_wrapper
.extern default_irq_handler

.global isr_irq1_wrapper
.extern irq1_handler

.global isr_irq12_wrapper
.extern irq12_handler

isr_default_wrapper:
    pusha; push %ds; push %es; push %fs; push %gs
    call default_irq_handler
    pop %gs; pop %fs; pop %es; pop %ds; popa
    iret

isr_irq1_wrapper:
    pusha; push %ds; push %es; push %fs; push %gs
    call irq1_handler
    pop %gs; pop %fs; pop %es; pop %ds; popa
    iret

isr_irq12_wrapper:
    pusha; push %ds; push %es; push %fs; push %gs
    call irq12_handler
    pop %gs; pop %fs; pop %es; pop %ds; popa
    iret