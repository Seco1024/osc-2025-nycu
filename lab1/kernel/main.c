#include "drivers/uart.h"
#include "kernel/shell.h"

int main() {
    uart_init();
    run_shell();
    return 0;
}