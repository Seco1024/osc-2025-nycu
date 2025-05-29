#ifndef _TIMER_H
#define _TIMER_H

extern void core_timer_enable(void);
extern void core_timer_disable(void);
extern unsigned long core_timer_handler(void);
extern unsigned long get_uptime(void);
extern void set_core_timer(unsigned long);

#endif