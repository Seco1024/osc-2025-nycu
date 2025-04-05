#ifndef _EXECEPTIONS_H
#define _EXECEPTIONS_H

#include "lib/stdlib.h"
#include "kernel/timer.h"
#include "kernel/timeout.h"
#include "lib/io.h"
#include "drivers/irq.h"
#include "drivers/aux.h"
#include "drivers/uart.h"
#include "kernel/task.h"

void default_handler(int type, unsigned long spsr, unsigned long elr, unsigned long esr);
void irq_handler(void);

void from_el1_to_el0(uint64_t, uint64_t);
void enable_irq(void);
void disable_irq(void);
int is_uart_rx_irq(void); 
int is_uart_tx_irq(void); 

#endif