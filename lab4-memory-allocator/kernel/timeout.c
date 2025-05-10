#include "kernel/interrupt_handler.h"
#include "kernel/task.h"
#include "kernel/mm.h"
#include "kernel/timer.h"
#include "drivers/uart.h"
#include "lib/stdlib.h"
#include "lib/string.h"
#include "lib/io.h"

void print_message(void *s) {
    if (!strcmp(s, "demo")) {
        char buf[16];
        unsigned long sec = get_uptime(); 
        itos(sec, buf);
        puts("Seconds since boot: ");
        puts(buf); 
        puts("\n");
        timer_demo(3, NULL);
    } else {
        puts("Message: ");
        puts((char *)s);
        puts("\n");
    }
    return ;
}

void set_timeout(int argc, char **argv) {
    unsigned long timestamp = get_uptime() + (unsigned long)atoi(argv[2]);
    task_t *new_task = simple_alloc(sizeof(task_t));
    new_task->function = print_message;
    new_task->timeout = timestamp;
    int msg_len = strlen(argv[1]);
    new_task->message = simple_alloc(msg_len + 1); 
    strcpy(new_task->message, argv[1]);

    add_timer_task(global_timer_queue, new_task);
    core_timer_enable();
} 

void timer_irq_handler(void) {
    if (global_timer_queue->size > 0) {
        task_t *task_handler = pop_timer_task(global_timer_queue);

        // Advance 1
        // char *message = task_handler->message;
        // task_handler->function(message);

        // Advance 2: Decoupling task execution and interrupt handling
        task_handler->timeout = 0;
        add_task(global_task_queue, task_handler);
    }
    return ;
}

void timer_demo(int argc, char **argv) {
    unsigned long timestamp = get_uptime() + TIMER_DEMO_TIMEOUT;
    task_t *new_task = simple_alloc(sizeof(task_t));
    new_task->function = print_message;
    new_task->timeout = timestamp;
    new_task->message = "demo";

    add_timer_task(global_timer_queue, new_task);
    core_timer_enable();
} 