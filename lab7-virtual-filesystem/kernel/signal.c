#include "kernel/signal.h"

extern void signal_return(void);

void signal(trap_frame *regs) {
    task_struct *current = get_current();
    if (current->signal_handling)
        return;

    int signum = 1;
    while (current->signal_pending) {
        if (current->signal_pending & 1) {
            current->signal_pending &= ~1;
            current->signal_handling = 1;

            if (current->signal_handlers[signum] == NULL) {
                thread_exit();
                return;
            }

            memncpy(&current->sigframe, regs, sizeof(trap_frame));
            current->signal_stack = allocate(STACK_SIZE);

            regs->elr_el1 = (unsigned long)current->signal_handlers[signum];
            regs->x30 = (unsigned long)signal_return;  
            regs->sp_el0   = (unsigned long)current->signal_stack + STACK_SIZE;  
            regs->spsr_el1 = 0;
            return;
        }
        current->signal_pending >>= 1;
        signum++;
    }
}

void syscall_sigreturn(trap_frame *regs) {
    memncpy(regs, &get_current()->sigframe, sizeof(trap_frame));
    free(get_current()->signal_stack);
    get_current()->signal_handling = 0;
    return;  
}