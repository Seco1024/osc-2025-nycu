#ifndef _TASK_H
#define _TASK_H

#define TASK_MAX_SIZE 30

typedef struct task_t {
    void (*function) (void *);
    unsigned long timeout;
    void *message;
} task_t;

typedef struct taskQueue {
    task_t *tasks[TASK_MAX_SIZE];
    int size;
} taskQueue;

extern taskQueue *global_timer_queue;
extern taskQueue *global_task_queue;

void task_queue_init(void);
void add_timer_task(taskQueue *, task_t *);
task_t *pop_timer_task(taskQueue *);
void add_task(taskQueue *, task_t *);
task_t *pop_task(taskQueue *);
void execute_tasks(taskQueue *);

#endif