#include "drivers/uart.h"
#include "lib/io.h"
#include "lib/stdlib.h"

#define BUFFER_SIZE 256


int main(void) {
    uart_init();
    uart_puts("Please input the size of kernel8.img: ");

    char *kernel = (char *)0x80000;
    char kernel_size_buffer[16];
    int idx = 0;
    unsigned int size;
    char c;

    while((c = uart_recv()) != '\n') {
        if (c < '0' || c > '9') {
            continue;
        }
        kernel_size_buffer[idx++] = c;
    }

    puts(kernel_size_buffer);
    puts(" bytes\n");

    puts("Loading kernel8.img...");
    size = atoi(kernel_size_buffer);

    while(size--) {
        *kernel++ = uart_recv();
    }

    asm volatile (
        "mov x0, x10; mov x30, 0x80000; ret;"
    );

    return 0;
}