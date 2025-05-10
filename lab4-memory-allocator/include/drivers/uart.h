#ifndef UART_H
#define UART_H 

#include "drivers/gpio.h"
#include "drivers/aux.h"
#include "drivers/irq.h"
#include "lib/io.h"
#include "kernel/interrupt_handler.h"

void uart_init(void);
char uart_getc(void);
void uart_putc(char);
void uart_puts(char *);
void async_puts(char *);
void async_putchar(char);
void async_gets(void); 

void enable_uart_interrupt(void);
void uart_rx_irq_handler(void);
void uart_tx_irq_handler(void);

void disable_uart_rx_interrupt(void);
void disable_uart_tx_interrupt(void);
void enable_uart_rx_interrupt(void);
void enable_uart_tx_interrupt(void);

extern char read_buffer[];
extern char write_buffer[];
extern int read_idx;
extern int write_idx;
extern int write_end;

#endif