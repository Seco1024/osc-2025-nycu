#include "drivers/uart.h"
#include "kernel/shell.h"
#include "kernel/memory.h"
#include "kernel/interrupt_handler.h"
#include "kernel/cpio.h"
#include "kernel/dtb.h"
#include "kernel/task.h"

int main() {
    uart_init();
    memory_init();
    fdt_traverse(ramfs_init);
    
    enable_irq();
    enable_uart_interrupt();
    task_queue_init();

    run_shell();
    return 0;
}