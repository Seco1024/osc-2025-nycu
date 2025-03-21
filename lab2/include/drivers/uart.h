#ifndef UART_H
#define UART_H 

void uart_init(void);
char uart_getc(void);
void uart_putc(char);
void uart_puts(char *);

#endif