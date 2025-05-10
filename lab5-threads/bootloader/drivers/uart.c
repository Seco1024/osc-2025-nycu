#include "drivers/uart.h"
#include "drivers/gpio.h"
#include "drivers/aux.h"

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

char uart_recv(void) {
    char c;
    do {
        asm volatile("nop");
    } while (!(*AUX_MU_LSR&0x01));
    c = (char)(*AUX_MU_IO);
    return c;
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