#include "kernel/registers.h"

.section ".text"

/* save general registers to stack */
.macro save_all
    sub sp, sp, 16 * 17
    stp x0, x1, [sp, 16 * 0]
    stp x2, x3, [sp, 16 * 1]
    stp x4, x5, [sp, 16 * 2]
    stp x6, x7, [sp, 16 * 3]
    stp x8, x9, [sp, 16 * 4]
    stp x10, x11, [sp, 16 * 5]
    stp x12, x13, [sp, 16 * 6]
    stp x14, x15, [sp, 16 * 7]
    stp x16, x17, [sp, 16 * 8]
    stp x18, x19, [sp, 16 * 9]
    stp x20, x21, [sp, 16 * 10]
    stp x22, x23, [sp, 16 * 11]
    stp x24, x25, [sp, 16 * 12]
    stp x26, x27, [sp, 16 * 13]
    stp x28, x29, [sp, 16 * 14]
    mrs x10, spsr_el1
    mrs x11, elr_el1
    mrs x12, sp_el0
    stp x30, x10, [sp, 16 * 15]
    stp x11, x12, [sp, 16 * 16]
.endm

/* load general registers from stack */
.macro load_all
    ldp x30, x10, [sp, 16 * 15]
    ldp x11, x12, [sp, 16 * 16]
    msr spsr_el1, x10
    msr elr_el1, x11
    msr sp_el0, x12
    ldp x0, x1, [sp, 16 * 0]
    ldp x2, x3, [sp, 16 * 1]
    ldp x4, x5, [sp, 16 * 2]
    ldp x6, x7, [sp, 16 * 3]
    ldp x8, x9, [sp, 16 * 4]
    ldp x10, x11, [sp, 16 * 5]
    ldp x12, x13, [sp, 16 * 6]
    ldp x14, x15, [sp, 16 * 7]
    ldp x16, x17, [sp, 16 * 8]
    ldp x18, x19, [sp, 16 * 9]
    ldp x20, x21, [sp, 16 * 10]
    ldp x22, x23, [sp, 16 * 11]
    ldp x24, x25, [sp, 16 * 12]
    ldp x26, x27, [sp, 16 * 13]
    ldp x28, x29, [sp, 16 * 14]
    add sp, sp, 16 * 17
.endm

.macro ventry label
    .align	7	// entry size is 0x80, .align will pad 0
    b	\label
.endm

.macro interrupt_handler label handler_function type
\label:
    save_all
    mov x0, sp
    bl \handler_function
    load_all
    eret
.endm

.macro exception_handler label handler_function type
\label:
    save_all
    mov x0, sp
    bl \handler_function
    load_all
    eret     
.endm

.global signal_return
signal_return:
    mov x8, 10
    svc 0

.global enable_irq
enable_irq:
    msr daifclr, 0xf
    ret

.global disable_irq
disable_irq:
    msr daifset, 0xf
    ret

.global from_el1_to_el0
from_el1_to_el0:
    msr elr_el1, x0  
    msr sp_el0, x1      
    ldr x2, =SPSR_EL1_ENABLED
    msr spsr_el1, x2  
    eret

.global child_ret_from_fork
child_ret_from_fork:
    load_all
    eret

.global set_exception_vector_table
set_exception_vector_table:
    ldr     x0, =exception_vector_table
    msr     vbar_el1, x0
    ret  
    
.align	11
.globl exception_vector_table 
exception_vector_table:
    // Current EL with SP0
    ventry	sync_invalid_el1t           // Synchronous EL1t
    ventry	irq_invalid_el1t            // IRQ EL1t
    ventry	fiq_invalid_el1t            // FIQ EL1t
    ventry	error_invalid_el1t          // Error EL1t

    // Current EL with SPx
    ventry	sync_invalid_el1h           // Synchronous EL1h
    ventry	irq_invalid_el1h            // IRQ EL1h
    ventry	fiq_invalid_el1h            // FIQ EL1h
    ventry	error_invalid_el1h          // Error EL1h

    // Lower EL using AArch64
    ventry	sync_invalid_el0t_64        // Synchronous 64-bit EL0
    ventry	irq_invalid_el0t_64         // IRQ 64-bit EL0
    ventry	fiq_invalid_el0t_64         // FIQ 64-bit EL0
    ventry	error_invalid_el0t_64       // Error 64-bit EL0

    // Lower EL using AArch32
    ventry	sync_invalid_el0t_32        // Synchronous 32-bit EL0
    ventry	irq_invalid_el0t_32         // IRQ 32-bit EL0
    ventry	fiq_invalid_el0t_32         // FIQ 32-bit EL0
    ventry	error_invalid_el0t_32       // Error 32-bit EL0


exception_handler     sync_invalid_el1t           default_handler     0  // EL1 -> EL1 while using SP_EL0
interrupt_handler     irq_invalid_el1t            default_handler     1
interrupt_handler     fiq_invalid_el1t            default_handler     2
interrupt_handler     error_invalid_el1t          default_handler     3
        
exception_handler     sync_invalid_el1h           trap_handler     4  // EL1 -> EL1 while using SP_EL1
interrupt_handler     irq_invalid_el1h            irq_handler         5
interrupt_handler     fiq_invalid_el1h            default_handler     6
interrupt_handler     error_invalid_el1h          default_handler     7
        
exception_handler     sync_invalid_el0t_64        trap_handler        8  // EL0 (AArch64) -> EL1
interrupt_handler     irq_invalid_el0t_64         irq_handler         9
interrupt_handler     fiq_invalid_el0t_64         default_handler     10
interrupt_handler     error_invalid_el0t_64       default_handler     11
        
exception_handler     sync_invalid_el0t_32        default_handler     12
interrupt_handler     irq_invalid_el0t_32         default_handler     13
interrupt_handler     fiq_invalid_el0t_32         default_handler     14
interrupt_handler     error_invalid_el0t_32       default_handler     15