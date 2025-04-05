#include "drivers/uart.h"

char read_buffer[BUFFER_SIZE];
char write_buffer[BUFFER_SIZE];
int read_idx = 0;
int write_idx = 0;
int write_end = 0;

/**
 * Initialize mini UART
 */
void uart_init(void) {
    // Set GPIO14, 15 to mini UART
    register unsigned int r = *GPFSEL1;
    r &= ~((7 << 12) | (7 << 15));    // Target GPIO14 & GPIO15
    r |= (2 << 12) | (2 << 15);   // Set to ALT5
    *GPFSEL1 = r;

    // Disable pull up/down
    *GPPUD = 0;
    r = 150;
    while(r--) asm volatile("nop");
    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    r = 150;
    while(r--) asm volatile("nop");
    *GPPUDCLK0 = 0;  

    // Initialize mini UART
    *AUX_ENB |= 1;          // Enable mini UART
    *AUX_MU_CNTL = 0;       // Disable transmitter and receiver during configuration
    *AUX_MU_IER = 0;        // Disable interrupt
    *AUX_MU_LCR = 3;        // Set the data size to 8 bit
    *AUX_MU_MCR = 0;        // Don’t need auto flow control
    *AUX_MU_BAUD = 270;     // Set Baud Rate to 115200
    *AUX_MU_IIR = 6;    
    *AUX_MU_CNTL = 3;       // Enable the transmitter and receiver
}

/**
 * Receive a character
 */
char uart_getc(void) {
    char c;
    do {
        asm volatile("nop");
    } while (!(*AUX_MU_LSR&0x01));
    c = (char)(*AUX_MU_IO);
    return c == '\r' ? '\n' : c;
}

/**
 * Display a character
 */
void uart_putc(char c) {
    do {
        asm volatile("nop");
    } while (!(*AUX_MU_LSR & 0x20));
    *AUX_MU_IO = c;
}

/**
 * Display a string
 */
void uart_puts(char *s) {
    while(*s) {
        if (*s == '\n') uart_putc('\r');
        uart_putc(*s++);
    }
}

void enable_uart_interrupt(void) {
    *AUX_MU_IER = 0x0;
    enable_uart_rx_interrupt();
    *ENABLE_IRQS_1 = 1 << 29;
}

void enable_uart_rx_interrupt(void) {
    *AUX_MU_IER |= 0x1;
}

void enable_uart_tx_interrupt(void) {
    *AUX_MU_IER |= 0x2;
}

void disable_uart_rx_interrupt(void) {
    *AUX_MU_IER &= 0x2;
}

void disable_uart_tx_interrupt(void) {
    *AUX_MU_IER &= 0x1;
}

void uart_rx_irq_handler(void) {
    char c = (char)(*AUX_MU_IO);
    read_buffer[read_idx++] = c;
    read_buffer[read_idx] = '\0';
}

void uart_tx_irq_handler(void) {
    disable_uart_tx_interrupt();
    char c;
    if ((c = write_buffer[--write_idx]) != '\0') {
        *AUX_MU_IO = c;
        if (write_idx == BUFFER_SIZE) {
            write_idx = 0;
        }
    }
}

void async_gets(void) {
    char c;
    int idx = 0;

    clear_buffer();

    while (1) {
        if (read_idx > 0) {
            c = read_buffer[--read_idx];
            if (c == 0x08 || c == 0x7F) {
                async_putchar('\b');
                async_putchar(' ');
                async_putchar('\b');
                buffer[--idx] = '\0';
                continue;
            }

            if (c == '\r' || c == '\n') {
                buffer[idx] = '\0';
                async_putchar('\n');
                async_putchar('\r');
                break;
            }
            else {
                buffer[idx++] = c;
                async_putchar(c);
            }
        }
    }
}

void async_putchar(char c) {
    write_buffer[write_idx++] = c;
    enable_uart_tx_interrupt();
}

void async_puts(char *s) {
    while(*s) {
        if (*s == '\n') async_putchar('\r');
        async_putchar(*s++);
    }
}