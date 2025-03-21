#include "drivers/uart.h"
#include "lib/io.h"

char buffer[BUFFER_SIZE];

char getchar(void) {
    return uart_getc();
}

char *gets(void) {
    char c;
    int idx = 0;

    clear_buffer();

    while((c = getchar()) != '\n') {
        if (c == 0x08 || c == 0x7F) {
            if (idx > 0) {
                putchar('\b');
                putchar(' ');
                putchar('\b');
                buffer[--idx] = '\0';
            }
            continue;
        }
        putchar(c);
        buffer[idx++] = c;
    }
    buffer[idx++] = '\0';
    puts("\n");

    return buffer;
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

void puthex64(unsigned long a) {
    uart_puts("0x");
    for (int i = 60; i >=0; i-=4) {
        uart_putc(HEX_CHARS[((a >> i) & 0xF)]);
    }
}

void clear_buffer(void) {
    for (int i = 0; i < BUFFER_SIZE; ++i) {
        buffer[i] = 0x0;
    }
}