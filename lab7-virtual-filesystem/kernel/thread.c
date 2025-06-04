#include "kernel/thread.h"
#include "kernel/mm.h"
#include "kernel/kmalloc.h"
#include "lib/io.h"
#include "lib/string.h"

static int id_counter = 0;
static task_struct *run_queue;

void enqueue(task_struct **q, task_struct *task) {
    if (*q == 0) {
        *q = task;
        task->next = task;
        task->prev = task;
    } else {
        task->next = *q;
        task->prev = (*q)->prev;
        (*q)->prev->next = task;
        (*q)->prev = task;
    }
}

void dequeue(task_struct **q, task_struct *task) {
    if (*q == task) 
        *q = (task->next == task) ? 0 : task->next;
    task->next->prev = task->prev;
    task->prev->next = task->next;
}

task_struct *thread_create(void (*function)()) {
    task_struct *thread = allocate(PAGE_SIZE);
    thread->pid = id_counter++;
    thread->state = TASK_RUNNING;
    thread->stack = allocate(STACK_SIZE);
    thread->user_stack = allocate(STACK_SIZE);
    memset(thread->signal_handlers, 0, sizeof(thread->signal_handlers));
    thread->signal_pending = 0;
    thread->signal_handling = 0;
    thread->context.lr = (unsigned long)function;
    thread->context.sp = (unsigned long)thread->stack + STACK_SIZE;
    thread->context.fp = (unsigned long)thread->stack + STACK_SIZE;
    enqueue(&run_queue, thread);

    strcpy(thread->cwd, "/");
    thread->fdtable.count = 0;
    for (int i = 0; i < MAX_FD; i++) {
        thread->fdtable.fds[i] = NULL;
    }
    return thread;
}

void schedule() {
    task_struct *current_thread = get_current();
    task_struct *next_thread = current_thread->next;
    while (next_thread->state != TASK_RUNNING) {
        next_thread = next_thread->next;
    }

    switch_to(current_thread, next_thread);
}

void kill_zombies() {
    task_struct *next = run_queue;
    task_struct *task = run_queue;
    do {
        next = task->next;
        if (task->state == TASK_TERMINATED) {
            dequeue(&run_queue, task);
            free(task->stack);
            free(task->user_stack);
        }
        task = next;
    } while (task != run_queue);
}

void idle() {
    while (1) {
        kill_zombies();
        schedule();
    }
}

void thread_init() {
    thread_create(idle);
    asm volatile("msr tpidr_el1, %0" ::"r"(run_queue));
}

void thread_exit() {
    task_struct *current_thread = get_current();
    current_thread->state = TASK_TERMINATED;
    schedule();
}

task_struct *get_thread(int pid) {
    task_struct *current = run_queue;
    do {
        if ((int)current->pid == pid)
            return current;
        current = current->next;
    } while (current != run_queue);
    return 0;
}

void thread_stop(int pid) {
    struct task_struct *task = run_queue;
    do {
        if (task->pid == pid) task->state = TASK_TERMINATED;
        task = task->next;
    } while (task != run_queue);
    schedule();
}

void foo() {
    for (int i = 0; i < 5; ++i) {
        puts("Thread id: ");
        puti(get_current()->pid);
        puts(" ");
        puti(i);
        puts("\n");
        for (int i = 0; i < 1000000; i++);
        schedule();
    }
    thread_exit();
}

void test_threads(int argc, char **argv) {
    for (int i = 0; i < 10; ++i) {
        thread_create(foo);
    }
}