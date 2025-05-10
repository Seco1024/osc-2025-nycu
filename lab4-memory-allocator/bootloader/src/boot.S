.section ".text.boot"

.global _start

_start:
    mov     x10, x0
    ldr     x1, =0x80000
    ldr     x2, =0x60000
    ldr     w3, =__loader_size

relocate:
    ldr     x4, [x1], #8
    str     x4, [x2], #8
    sub     w3, w3, #1
    cbnz    w3, relocate

master:  
    ldr     x1, =0x60000
    mov     sp, x1
    ldr     x1, =__bss_begin
    ldr     w2, =__bss_size

clear:
    cbz     w2, boot_main
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, clear

boot_main:
    bl main-0x20000