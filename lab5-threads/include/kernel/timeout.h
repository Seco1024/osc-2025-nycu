#ifndef _TIMEOUT_H
#define _TIMEOUT_H

#define TIMER_DEMO_TIMEOUT 1

void timer_demo(int argc, char **argv);
void disable_timer_demo(int argc, char **argv);
void set_timeout(int argc, char **argv);
void timer_irq_handler(void);
void enable_user_to_physical_timer(void);

#endif