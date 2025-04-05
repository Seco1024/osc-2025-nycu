#include "kernel/task.h"
#include "kernel/memory.h"
#include "kernel/timer.h"
#include "lib/io.h"
#include "lib/stdlib.h"

taskQueue *global_timer_queue;
taskQueue *global_task_queue;

void add_timer_task(taskQueue *q, task_t *task) {
    if (q->size >= TASK_MAX_SIZE) return;
    int i = q->size++;
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (q->tasks[parent]->timeout <= task->timeout) break;
        q->tasks[i] = q->tasks[parent];
        i = parent;
    }

    q->tasks[i] = task;

    unsigned long target_timestamp = q->tasks[0]->timeout;
    unsigned long uptime = get_uptime();
    
    set_core_timer(target_timestamp - uptime);
    
}

task_t *pop_timer_task(taskQueue *q) {
    if (q->size == 0) return (void *)0;

    task_t *top = q->tasks[0];
    task_t *last = q->tasks[--q->size];

    int i = 0; 
    while (i * 2 + 1 < q->size) {
        int child = i * 2 + 1;
        if (child + 1 < q->size &&
            q->tasks[child + 1]->timeout < q->tasks[child]->timeout)
            child++;

        if (last->timeout <= q->tasks[child]->timeout) break;

        q->tasks[i] = q->tasks[child];
        i = child;
    }
    q->tasks[i] = last;

    if (q->size > 0) {
        unsigned long target_timestamp = q->tasks[0]->timeout;
        unsigned long uptime = get_uptime();
        unsigned long delay = (target_timestamp > uptime) ? (target_timestamp - uptime) : 0;
        set_core_timer(delay);
    }
        
    return top;
}

void add_task(taskQueue *q, task_t *task) {
    if (q->size >= TASK_MAX_SIZE) return;
    int i = q->size++;
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (q->tasks[parent]->timeout <= task->timeout) break;
        q->tasks[i] = q->tasks[parent];
        i = parent;
    }
    q->tasks[i] = task;
}

task_t *pop_task(taskQueue *q) {
    if (q->size == 0) return (void *)0;

    task_t *top = q->tasks[0];
    task_t *last = q->tasks[--q->size];

    int i = 0; 
    while (i * 2 + 1 < q->size) {
        int child = i * 2 + 1;
        if (child + 1 < q->size &&
            q->tasks[child + 1]->timeout < q->tasks[child]->timeout)
            child++;

        if (last->timeout <= q->tasks[child]->timeout) break;

        q->tasks[i] = q->tasks[child];
        i = child;
    }
    q->tasks[i] = last;
    return top;
}

void execute_tasks(taskQueue *q) {
    while (q->size > 0) {
        task_t *task_handler = pop_task(global_task_queue);
        char *message = task_handler->message;
        task_handler->function(message);
    }
}

void task_queue_init(void) {
    global_timer_queue = simple_alloc(sizeof(taskQueue));
    global_timer_queue->size = 0;
    global_task_queue = simple_alloc(sizeof(taskQueue));
    global_task_queue->size = 0;
}