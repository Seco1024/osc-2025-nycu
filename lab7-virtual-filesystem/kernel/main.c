#include "drivers/uart.h"
#include "kernel/shell.h"
#include "kernel/mm.h"
#include "kernel/buddy.h"
#include "kernel/interrupt_handler.h"
#include "kernel/cpio.h"
#include "kernel/dtb.h"
#include "kernel/thread.h"
#include "kernel/task.h"
#include "kernel/kmalloc.h"
#include "kernel/slab.h"
#include "kernel/vfs.h"

extern uint32_t _dtb_base;  

int main() {
    // Peripherals Initialization
    uart_init();
    fdt_traverse(ramfs_init);
    
    // Interrupt Handling
    memory_init();
    enable_irq();
    // enable_uart_interrupt();
    task_queue_init();
    core_timer_enable();

    // Memory Allocator
    buddy_init((uintptr_t)0x0, (uintptr_t)0x3C000000);
    slab_init();

    memory_reserve((uintptr_t)0x0, (uintptr_t)(0x1000 - 1));                                                      // Spin Tables for Multiple Cores
    memory_reserve((uintptr_t)0x80000, (uintptr_t)(HEAP_MAX - 1));                                                // Text & Startup Allocator
    memory_reserve((uintptr_t)_dtb_base, (uintptr_t)(_dtb_end - 1));                                              // Device Tree
    memory_reserve((uintptr_t)CPIO_BASE, (uintptr_t)(CPIO_BASE + 0x100000 - 1));                                  // Initramfs
    memory_reserve((uintptr_t)USER_STACK_POINTER_BASE, (uintptr_t)(USER_STACK_POINTER_BASE + 0x10000 - 1));       // User Stack

    // rootfs
    init_rootfs();

    // thread
    thread_init();
    // thread_create(run_shell);
    run_shell();
    
    // idle();
    return 0;
}