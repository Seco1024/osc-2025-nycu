#include "kernel/registers.h"

.extern print_debug
.section ".text"

.global core_timer_enable
core_timer_enable:
    mov x0, 1
    msr cntp_ctl_el0, x0 
    mrs x0, cntfrq_el0
    msr cntp_tval_el0, x0
    mov x0, 2
    ldr x1, =0xffff000040000040
    str w0, [x1] 
    
    mrs x0, cntkctl_el1        
    orr x0, x0, 1          
    msr cntkctl_el1, x0     
    ret

.global core_timer_disable
core_timer_disable:
    mov x0, 0
    msr cntp_ctl_el0, x0 


.global core_timer_handler
core_timer_handler:
    mrs x0, cntfrq_el0
    lsl x1, x0, #1
    msr cntp_tval_el0, x1
    mrs x1, cntpct_el0
    udiv x0, x1, x0       
    ret

.global set_core_timer
set_core_timer:
    mrs x1, cntfrq_el0     
    mul x0, x0, x1         
    msr cntp_tval_el0, x0   
    ret

.global get_uptime
get_uptime:
    mrs x1, cntfrq_el0
    mrs x0, cntpct_el0
    udiv x0, x0, x1 
    ret

.global set_shedule_timer
set_shedule_timer:
    mrs x0, cntfrq_el0  
    mov x1, #32         
    udiv x0, x0, x1   
    msr cntp_tval_el0, x0
    mov x0, #1
    msr cntp_ctl_el0, x0
    ret