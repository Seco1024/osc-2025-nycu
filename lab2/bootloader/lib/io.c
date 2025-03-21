#include "drivers/uart.h"
#include "lib/io.h"

char getchar(void) {
    return uart_getc();
}

void puts(char *s) {
    return uart_puts(s);
}

void putchar(char c) {
    return uart_putc(c);
}

void puthex(unsigned int a) {
    uart_puts("0x");
    for (int i = 28; i >=0; i-=4) {
        uart_putc(HEX_CHARS[((a >> i) & 0xF)]);
    }
}