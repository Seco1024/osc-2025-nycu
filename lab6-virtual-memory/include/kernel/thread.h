#ifndef _SCHED_H
#define _SCHED_H

#define STACK_SIZE 0x4000
#define NSIG 10
#include "kernel/traps.h"

typedef struct thread_context {
    unsigned long x19;
    unsigned long x20;
    unsigned long x21;
    unsigned long x22;
    unsigned long x23;
    unsigned long x24;
    unsigned long x25;
    unsigned long x26;
    unsigned long x27;
    unsigned long x28;
    unsigned long fp;
    unsigned long lr;
    unsigned long sp;
    unsigned long pgd;
} thread_context;

enum task_state {
    TASK_RUNNING,
    TASK_PENDING,
    TASK_TERMINATED,
};

typedef struct task_struct {
    thread_context context;
    unsigned long pid;
    enum task_state state;
    void *code;
    int code_size;
    void *stack;
    void *user_stack;
    struct task_struct *prev;
    struct task_struct *next;
    void (*signal_handlers[NSIG + 1])();
    void *signal_stack;
    int signal_pending;
    int signal_handling;
    trap_frame sigframe;
} task_struct;

extern task_struct *get_current();
extern void switch_to(task_struct*, task_struct*);

void test_threads(int, char **);
void thread_init(void);
void schedule(void);
task_struct *thread_create(void (*function)());
void thread_exit(void);
void thread_stop(int);
task_struct *get_thread(int);
void idle(void);

#endif