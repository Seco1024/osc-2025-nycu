#include "kernel/interrupt_handler.h"
#include "kernel/thread.h"
#include "kernel/traps.h"
#include "kernel/syscall.h"
#include "kernel/signal.h"

extern task_struct *get_current();
extern void set_shedule_timer();

int is_uart_rx_irq(void) {
    return (*AUX_MU_IIR & 0x06) == 0x04;
}

int is_uart_tx_irq(void) {
    return (*AUX_MU_IIR & 0x06) == 0x02;
}

void default_handler(int type, unsigned long spsr, unsigned long elr, unsigned long esr) {
    puts("SPSR: 0x");
    puthex64(spsr);
    puts("\nELR: 0x");
    puthex64(elr);
    puts("\nESR: 0x");
    puthex64(esr);
    puts("\n");
    return ;
}

void trap_handler(trap_frame *frame) {
    unsigned long elr, esr, spsr;
    asm volatile("mrs %0, elr_el1" : "=r"(elr));
    asm volatile("mrs %0, esr_el1" : "=r"(esr));
    asm volatile("mrs %0, spsr_el1" : "=r"(spsr));
    if ((esr >> 26) != 0x15) {
        thread_exit();
    }

    enable_irq();
    switch (frame->x8) {
        case 0:
            frame->x0 = syscall_getpid();
            break;
        case 1:
            frame->x0 = syscall_uart_read((char *)frame->x0, frame->x1);
            break;
        case 2:
            frame->x0 = syscall_uart_write((const char *)frame->x0, frame->x1);
            break;
        case 3:
            frame->x0 = syscall_exec((const char *)frame->x0, (char *const *)frame->x1);
            frame->elr_el1 = get_current()->context.lr;
            frame->sp_el0 = (unsigned long)get_current()->user_stack + STACK_SIZE;
            break;
        case 4:
            frame->x0 = syscall_fork(frame);  
            break;
        case 5:
            syscall_exit();
            break;
        case 6:
            frame->x0 = syscall_mbox_call(frame->x0, (unsigned int *)frame->x1);
            break;
        case 7:
            syscall_kill(frame->x0);
            break;
        case 8:
            syscall_signal(frame->x0, (void (*))frame->x1);
            break;
        case 9:
            syscall_sigkill(frame->x0, frame->x1);
            break;
        case 10:
            syscall_sigreturn(frame);
            break;
        case 11:
            syscall_open(frame, (char*)frame->x0, frame->x1);      
            break;
        case 12:
            syscall_close(frame, frame->x0);
            break;
        case 13: 
            syscall_write(frame, frame->x0, (char *)frame->x1, frame->x2);
            break;
        case 14:
            syscall_read(frame, frame->x0, (char *)frame->x1, frame->x2);
            break;
        case 15:
            syscall_mkdir(frame, (char *)frame->x0, frame->x1);
            break;
        case 16:
            syscall_mount(frame, (char *)frame->x0, (char *)frame->x1, (char *)frame->x2, frame->x3, (void*)frame->x4);
            break;
        case 17:
            syscall_chdir(frame, (char *)frame->x0);
            break;
        case 18:
            syscall_lseek64(frame, frame->x0, frame->x1, frame->x2);
            break;
        default:
            break;
    }
}

void irq_handler(trap_frame *regs) {
    disable_irq(); 
    if (*CORE0_IRQ_SOURCE == IS_CORE_TIMER_IRQ) {      // timer interrupt
        schedule();
        core_timer_disable();
        timer_irq_handler();
        set_shedule_timer();
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
    signal(regs);
    enable_irq();
    // execute_tasks(global_task_queue);
}