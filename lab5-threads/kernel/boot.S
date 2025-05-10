#include "kernel/registers.h"

.section ".data"
.global _dtb_base
_dtb_base: .word 0

.section ".text.boot"
.global _start

_start:
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, master

hang:
    wfe
    b       hang

master:  
    ldr     x1, =_dtb_base
    str     x0, [x1]


from_el2_to_el1:
    ldr     x0, =HCR_EL2_VALUE 
    msr     hcr_el2, x0
    ldr     x0, =SPSR_EL2_VALUE   
    msr     spsr_el2, x0    
    ldr     x0, =set_exception_vector_table     
    msr     elr_el2, x0
    eret 

set_exception_vector_table:
    ldr     x0, =exception_vector_table
    msr     vbar_el1, x0

irq_setup:
    bl      enable_irq

clear_bss:
    ldr     x1, =_start
    mov     sp, x1
    ldr     x1, =__bss_begin
    ldr     x2, =__bss_size
    cbz     x2, kernel

clear_bss_loop:
    str     xzr, [x1], #8
    sub     x2, x2, #1
    cbnz    x2, clear_bss_loop

.global kernel
kernel:
    bl      main
    b       hang