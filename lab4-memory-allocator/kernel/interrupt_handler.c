#include "kernel/interrupt_handler.h"

int is_uart_rx_irq(void) {
    return (*AUX_MU_IIR & 0x06) == 0x04;
}

int is_uart_tx_irq(void) {
    return (*AUX_MU_IIR & 0x06) == 0x02;
}

void default_handler(int type, unsigned long spsr, unsigned long elr, unsigned long esr) {
    // puts("SPSR: 0x");
    // puthex64(spsr);
    // puts("\nELR: 0x");
    // puthex64(elr);
    // puts("\nESR: 0x");
    // puthex64(esr);
    // puts("\n");
}

void irq_handler(void) {
    disable_irq(); 
    if (*CORE0_IRQ_SOURCE == IS_CORE_TIMER_IRQ) {      // timer interrupt
        core_timer_disable();
        timer_irq_handler();
        core_timer_enable();
        
    } else if (*IRQ_PENDING_1 & IS_AUX_IRQ) {          // uart interrupt
        if (is_uart_rx_irq()) {
            disable_uart_rx_interrupt();
            uart_rx_irq_handler();
            enable_uart_rx_interrupt();
        } else if (is_uart_tx_irq()) {
            disable_uart_tx_interrupt();
            uart_tx_irq_handler();
        }
    }
    enable_irq();
    execute_tasks(global_task_queue);
}