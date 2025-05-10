#include "drivers/uart.h"
#include "kernel/shell.h"
#include "kernel/mm.h"
#include "kernel/buddy.h"
#include "kernel/interrupt_handler.h"
#include "kernel/cpio.h"
#include "kernel/dtb.h"
#include "kernel/task.h"
#include "kernel/kmalloc.h"
#include "kernel/slab.h"

extern uint32_t _dtb_base;  

int main() {
    // Peripherals Initialization
    uart_init();
    fdt_traverse(ramfs_init);
    
    // Interrupt Handling
    memory_init();
    enable_irq();
    enable_uart_interrupt();
    task_queue_init();

    // Memory Allocator
    buddy_init((uintptr_t)0x0, (uintptr_t)0x3C000000);
    slab_init();

    memory_reserve((uintptr_t)0x0, (uintptr_t)(0x1000 - 1));                                                      // Spin Tables for Multiple Cores
    memory_reserve((uintptr_t)0x80000, (uintptr_t)(HEAP_MAX - 1));                                                // Text & Startup Allocator
    memory_reserve((uintptr_t)_dtb_base, (uintptr_t)(_dtb_end - 1));                                              // Device Tree
    memory_reserve((uintptr_t)CPIO_BASE, (uintptr_t)(CPIO_BASE + 0x100000 - 1));                                  // Initramfs
    memory_reserve((uintptr_t)USER_STACK_POINTER_BASE, (uintptr_t)(USER_STACK_POINTER_BASE + 0x10000 - 1));       // User Stack

    // // test
    // void *p1 = allocate(4096); 
    // void *p2 = allocate(6144); 
    // free(p1);
    // free(p2);
    // void *p3 = allocate(4096); 
    // void *p4 = allocate(2147483647); 
    // free(p4);
    // free(p3);
    // void *p6 = allocate(16); 
    // void *p7 = allocate(33); 
    // void *p8 = allocate(63); 
    // free(p8);
    // free(p7);
    // free(p6);
    // puts("\n==========\n");

    // printf("\n==========\n");

    // // Allocate all blocks at order 0, 1, 2 and 3
    // void *ps0[NUM_BLOCKS_AT_ORDER_0];
    // void *ps1[NUM_BLOCKS_AT_ORDER_1];
    // void *ps2[NUM_BLOCKS_AT_ORDER_2];
    // void *ps3[NUM_BLOCKS_AT_ORDER_3];
    // for (int i = 0; i < NUM_BLOCKS_AT_ORDER_0; ++i) {
    //     ps0[i] = alloc(4096);
    // }
    // for (int i = 0; i < NUM_BLOCKS_AT_ORDER_1; ++i) {
    //     ps1[i] = alloc(8192);
    // }
    // for (int i = 0; i < NUM_BLOCKS_AT_ORDER_2; ++i) {
    //     ps2[i] = alloc(16384);
    // }
    // for (int i = 0; i < NUM_BLOCKS_AT_ORDER_3; ++i) {
    //     ps3[i] = alloc(32768);
    // }


    // MAX_BLOCK_SIZE = PAGE_SIZE * (2 ** MAX_ORDER);

    /* **DO NOT** uncomment this section */
    void *p1 = allocate(4095);                // [Page] Allocate x at order 0, page y
    free(p1);                        // [Page] Free x at order 0, page y
    p1 = allocate(4095);                // [Page] Allocate x at order 0, page y

    void *c1 = allocate(1000);                // [Page] Allocate x at order 0, page y
    void *c2 = allocate(1023);                // [Page] Allocate x at order 0, page y
    void *c3 = allocate(999);                 // [Page] Allocate x at order 0, page y
    void *c4 = allocate(1010);                // [Page] Allocate x at order 0, page y
    free(c3);                        // [Page] Free x at order 0, page y
    void *c5 = allocate(989);                 // [Page] Allocate x at order 0, page y
    c3 = allocate(88);                  // [Chunk] Allocate x at chunk size 128
    void *c6 = allocate(1001);                // [Page] Allocate x at order 0, page y
    free(c3);                        //  [Page] Free x at order 0, page y
    void *c7 = allocate(2045);                //  [Page] Allocate x at order 0, page y
    void *c8 = allocate(1);                   //  [Chunk] Allocate x at order 0, page y

    void *p2 = allocate(4096);                // [Page] Allocate x at order 0, page y
    free(c8);                        //  [Page] Free x at order 0, page y
    void *p3 = allocate(16000);               //  [Page] Free x at order 2, page y
    free(p1);                        // 4095
    free(c3);                        // 88
    free(c7);                        // 2045
    void *p4 = allocate(4097);                // [Page] Allocate x at order 1, page y
    void *p5 = allocate(2147483648);  // no print
    void *p6 = allocate(2147483647);      // no print
    free(p2);                        // [Page] Free x at order 0, page y
    free(p5);                        // no print
    free(p4);                        // [Page] Free x at order 0, page y
    void *p7 = allocate(7197);                // [Page] Allocate x at order 1, page y

    free(p6);                        // no print
    free(p3);                        // [Page] Free x at order 2, page y
    free(p7);                        // [Page] Free x at order 1, page y
    free(c1);                        // [Page] Free x at order 0, page y
    free(c6);                        // [Page] Free x at order 0, page y
    free(c2);                        // [Page] Free x at order 0, page y
    free(c5);                        // [Page] Free x at order 0, page y
    free(c4);                        // [Page] Free x at order 0, page y

    // Free all blocks remaining
    // for (int i = 0; i < NUM_BLOCKS_AT_ORDER_0; ++i) {
    //     free(ps0[i]);
    // }
    // for (int i = 0; i < NUM_BLOCKS_AT_ORDER_1; ++i) {
    //     free(ps1[i]);
    // }
    // for (int i = 0; i < NUM_BLOCKS_AT_ORDER_2; ++i) {
    //     free(ps2[i]);
    // }
    // for (int i = 0; i < NUM_BLOCKS_AT_ORDER_3; ++i) {
    //     free(ps3[i]);
    // }
    // puts("\n==========\n");

    run_shell();
    return 0;
}