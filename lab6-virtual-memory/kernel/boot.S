#include "kernel/registers.h"
#include "kernel/mmu.h"

.section ".data"
.global _dtb_base
_dtb_base: .space 8

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
    mov     x19, x0
    bl      from_el2_to_el1
    ldr     x1, =0x7F000   
    mov     sp, x1      

mmu_enable:
    /* enable mmu */
    ldr x1, = TCR_CONFIG_DEFAULT
    msr tcr_el1, x1

    ldr x1, = ((MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)))
    msr mair_el1, x1

    mov x1, 0x1000 // PGD's page frame at 0x1000
    mov x2, 0x2000 // PUD's page frame at 0x2000

    ldr x3, = BOOT_PGD_ATTR
    orr x3, x2, x3 // combine the physical address of next level page with attribute.
    str x3, [x1]

    ldr x3, = BOOT_PGD_ATTR
    mov x4, 0x3000 // set PMD's page frame at 0x3000, used to fine grain 0x0~0x3FFFFFFF(RAM and GPU peripherals)
    orr x4, x3, x4
    str x4, [x2]// 1st 1GB mapped by the 1st entry of PUD
    ldr x3, = BOOT_DEVICE_ATTR
    mov x4, 0x40000000 // should be device memory
    orr x4, x3, x4
    str x4, [x2, 8]// 2nd 1GB mapped by the 2nd entry of PUD

    mov x2, 0x00000000
    mov x3, 0x3f000000
    ldr x4, = BOOT_NORMAL_ATTR
    ldr x5, = BOOT_DEVICE_ATTR
    mov x6, 512 // 512 entries in a PMD (1GB/2MB = 512)
    mov x7, 0x3000 // PMD's address

fine_grain_ram_mmio:
    cbz x6, boot_mmu
    cmp x2, x3
    bge mmio_region

normal_ram:
    orr x8, x2, x4
    str x8, [x7], #8
    add x2, x2, #0x200000 // 2MB
    sub x6, x6, 1
    b fine_grain_ram_mmio

mmio_region:
    orr x8, x2, x5
    str x8, [x7], #8
    add x2, x2, #0x200000 // 2MB
    sub x6, x6, 1
    b fine_grain_ram_mmio

boot_mmu:
    msr ttbr0_el1, x1 // load PGD to the bottom translation-based register.
    msr ttbr1_el1, x1 // also load PGD to the upper translation based register.
    mrs x3, sctlr_el1
    orr x3, x3, 1
    msr sctlr_el1, x3 // enable MMU, cache remains disabled

    ldr x2, = boot_rest // indirect branch to the virtual address
    br x2

boot_rest:
    ldr     x1, =_dtb_base   // x1 gets virtual address of _dtb_base
    str     x19, [x1]        // Store saved physical DTB pointer
    bl      enable_irq
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

from_el2_to_el1:
    mov     x4, (1<<31) 
    msr     hcr_el2, x4 
    mov     x4, 0x3c5 
    msr     spsr_el2, x4 
    msr     elr_el2, lr
    eret 