#ifndef SIGNAL_H
#define SIGNAL_H

#include "kernel/traps.h"
#include "kernel/mm.h"
#include "kernel/kmalloc.h"
#include "kernel/thread.h"
#include "lib/string.h"

void signal(trap_frame *);
void syscall_sigreturn(trap_frame *);

#endif 
