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
    mov     sp, #0x400000
    ldr     x1, =__bss_begin
    ldr     x2, =__bss_size

clear:
    cbz     x2, kernel
    str     xzr, [x1], #8
    sub     x2, x2, #1
    cbnz    x2, clear

kernel:
    bl      main
    b       hang