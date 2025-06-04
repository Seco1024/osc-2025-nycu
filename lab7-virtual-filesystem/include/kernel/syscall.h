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

int syscall_open(trap_frame *tf, const char *pathname, int flags);
int syscall_close(trap_frame *tf, int fd);
long syscall_write(trap_frame *tf, int fd, const void *buf, unsigned long count);
long syscall_read(trap_frame *tf, int fd, void *buf, unsigned long count);
long syscall_mkdir(trap_frame *tf, const char *pathname, unsigned mode);
long syscall_mount(trap_frame *tf, const char *src, const char *target, const char *filesystem, unsigned long flags, const void *data);
long syscall_chdir(trap_frame *tf, const char *path);
long syscall_lseek64(trap_frame *tf, int fd, long offset, int whence);

int getpid(void);
void exit(void);
int fork(void);
int exec(const char *path, char *const argv[]);


#endif