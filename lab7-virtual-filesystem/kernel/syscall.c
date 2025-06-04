#include "kernel/syscall.h"
#include "kernel/thread.h"
#include "kernel/interrupt_handler.h"
#include "lib/io.h"
#include "lib/string.h"
#include "kernel/cpio.h"
#include "kernel/mm.h"
#include "kernel/kmalloc.h"
#include "drivers/uart.h"
#include "drivers/mailbox.h"

int syscall_getpid(void) { 
    return get_current()->pid; 
}

int syscall_uart_read(char *buf, int size) {
    int i = 0;
    while (i < size) {
        buf[i++] = uart_getc();
    }
    return i;
}

int syscall_uart_write(const char *buf, int size) {
    int i = 0;
    while (i < size) uart_putc(buf[i++]);
    return i;
}

int syscall_exec(const char *file_name, char *const argv[]) {
    // unsigned long file_size = ramfs_get_file_size(file_name);
    // char *file_contents = allocate(file_size);
    // ramfs_get_file_contents(file_name, file_contents);

    // if (file_contents == NULL) {
    //     puts("[ERROR] File not found\n");
    //     puts("\n");
    //     return 0;
    // }

    struct file *file;
    int ret = vfs_open(file_name, O_RDWR, &file);
    if (ret == -1) {
        puts("\r\nCannot open file");
        return -1;
    }

    int filesize = file->vnode->f_ops->getsize(file->vnode);
    void *user_program = allocate(filesize);

    if (user_program == NULL) {
        puts("\r\nCannot allocate memory for file");
        return -1;
    }

    vfs_read(file, user_program, filesize);
    vfs_close(file);
    get_current()->state = TASK_PENDING;
    vfs_read(file, user_program, filesize);
    get_current()->signal_pending = 0;
    memset(get_current()->signal_handlers, 0, sizeof(get_current()->signal_handlers));
    get_current()->context.lr = (unsigned long)user_program;
    return 0;
}

void syscall_exit() { 
    thread_exit(); 
}

int syscall_fork(trap_frame *frame) {
    // disable_irq();
    struct task_struct *parent = get_current();
    struct task_struct *child = thread_create(0);

    memncpy(child->stack, parent->stack, STACK_SIZE);
    memncpy(child->user_stack, parent->user_stack, STACK_SIZE); 
    memncpy(child->signal_handlers, parent->signal_handlers, sizeof(parent->signal_handlers));

    unsigned long sp_off = (unsigned long)frame - (unsigned long)parent->stack;
    trap_frame *child_trap_frame = (trap_frame *)(child->stack + sp_off);

    child->context.sp = (unsigned long)child_trap_frame;
    child->context.lr = (unsigned long)child_ret_from_fork;

    unsigned long sp_el0_off = frame->sp_el0 - (unsigned long)parent->user_stack;
    child_trap_frame->sp_el0 = (unsigned long)child->user_stack + sp_el0_off;
    child_trap_frame->x0 = 0;

    // enable_irq();
    strcpy(child->cwd, parent->cwd);
    for (int i = 0; i < MAX_FD; ++i) {
        if (parent->fdtable.fds[i]) {
            child->fdtable.fds[i] = allocate(sizeof(struct file));
            *child->fdtable.fds[i] = *parent->fdtable.fds[i];
        }
    }
    return child->pid;
}

int syscall_mbox_call(unsigned char ch, unsigned int *mbox) {
    return sys_mailbox_call(ch, mbox);
}

void syscall_kill(int pid) {
    thread_stop(pid); 
}

void syscall_signal(int signum, void (*handler)()) {
    get_current()->signal_handlers[signum] = handler;
}

void syscall_sigkill(int pid, int signo) {
    task_struct *target = get_thread(pid);
    if (!target) return;
    target->signal_pending |= 1u << (signo - 1);        
}


int syscall_open(trap_frame *tf, const char *pathname, int flags) {
    char abs_path[MAX_PATH_NAME];
    strcpy(abs_path, pathname);
    get_absolute_path(abs_path, get_current()->cwd);
    for (int i = 0; i < MAX_FD; i++) {
        if (!get_current()->fdtable.fds[i]) {
            if (vfs_open(abs_path, flags, &get_current()->fdtable.fds[i]) != 0) {
                break;
            }

            tf->x0 = i;
            return i;
        }
    }

    tf->x0 = -1;
    return -1;
}

int syscall_close(trap_frame *tf, int fd) {
    if (get_current()->fdtable.fds[fd]) {
        vfs_close(get_current()->fdtable.fds[fd]);
        get_current()->fdtable.fds[fd] = 0;
        tf->x0 = 0;
        return 0;
    }

    tf->x0 = -1;
    return -1;
}

long syscall_write(trap_frame *tf, int fd, const void *buf, unsigned long count) {
    if (get_current()->fdtable.fds[fd]) {
        tf->x0 = vfs_write(get_current()->fdtable.fds[fd], buf, count);
        return tf->x0;
    }

    tf->x0 = -1;
    return tf->x0;
}

long syscall_read(trap_frame *tf, int fd, void *buf, unsigned long count) {
    if (get_current()->fdtable.fds[fd]) {
        tf->x0 = vfs_read(get_current()->fdtable.fds[fd], buf, count);
        return tf->x0;
    }

    tf->x0 = -1;
    return tf->x0;
}

long syscall_mkdir(trap_frame *tf, const char *pathname, unsigned mode) {
    char abs_path[MAX_PATH_NAME];
    strcpy(abs_path, pathname);
    get_absolute_path(abs_path, get_current()->cwd);
    tf->x0 = vfs_mkdir(abs_path);
    return tf->x0;
}

long syscall_mount(trap_frame *tf, const char *src, const char *target,
               const char *filesystem, unsigned long flags, const void *data) {
    char abs_path[MAX_PATH_NAME];
    strcpy(abs_path, target);

    tf->x0 = vfs_mount(abs_path, filesystem);
    return tf->x0;
}

long syscall_chdir(trap_frame *tf, const char *path) {
    char abs_path[MAX_PATH_NAME];
    strcpy(abs_path, path);
    get_absolute_path(abs_path, get_current()->cwd);
    strcpy(get_current()->cwd, abs_path);

    return 0;
}

long syscall_lseek64(trap_frame *tf, int fd, long offset, int whence) {
    if (whence == SEEK_SET) {
        get_current()->fdtable.fds[fd]->f_pos = offset;
        tf->x0 = offset;
    } else {
        tf->x0 = -1;
    }

    return tf->x0;
}

int getpid() {
    long pid = -1;
    asm volatile("mov x8, 0");
    asm volatile("svc 0");
    asm volatile("mov %0, x0" : "=r"(pid));
    return pid;
}

int exec(const char *path, char *const argv[]) {
    long ret;
    asm volatile("mov x0, %0" :: "r"(path));
    asm volatile("mov x1, %0" :: "r"(argv));
    asm volatile("mov x8, 3");
    asm volatile("svc 0");
    asm volatile("mov %0, x0" : "=r"(ret));

    return (int)ret;
}


int fork() {
    long ret = -1;
    asm volatile("mov x8, 4");
    asm volatile("svc 0");
    asm volatile("mov %0, x0" : "=r"(ret));
    return ret;
}

void exit() {
    asm volatile("mov x8, 5");
    asm volatile("svc 0");
}