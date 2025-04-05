#include "drivers/uart.h"
#include "kernel/shell.h"
#include "kernel/memory.h"
#include "kernel/cpio.h"
#include "kernel/dtb.h"

int main() {
    uart_init();
    memory_init();
    fdt_traverse(ramfs_init);
    run_shell();
    return 0;
}