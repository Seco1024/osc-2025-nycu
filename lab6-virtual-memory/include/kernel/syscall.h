#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "kernel/traps.h"

#define __NR_syscalls	8

extern void child_ret_from_fork();

int syscall_getpid(void);
void syscall_exit(void);
int syscall_fork(trap_frame *);
int syscall_exec(const char *file_name, char *const argv[]);
int syscall_uart_read(char *, int);
int syscall_uart_write(const char *, int);
int syscall_mbox_call(unsigned char, unsigned int *);
void syscall_kill(int);
void syscall_signal(int signum, void (*handler)());
void syscall_sigkill(int pid, int signo);

int getpid(void);
void exit(void);
int fork(void);
int exec(const char *path, char *const argv[]);


#endif