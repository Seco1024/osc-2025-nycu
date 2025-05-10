#ifndef _IRQ_H
#define AUX_IRQ_H_H

#include "gpio.h"

#define ENABLE_IRQS_1        ((volatile unsigned int*)(PBASE+0x0000B210))
#define DISABLE_IRQS_1       ((volatile unsigned int*)(PBASE+0x0000B21C))

#define CORE0_IRQ_SOURCE     ((volatile unsigned int*)(0x40000060))          // Get the IRQ source for the core 0 (QA7 page 16)
#define IRQ_BASIC_PENDING    ((volatile unsigned int*)(PBASE+0x0000B200))    // BCM2835 page 112-116
#define IRQ_PENDING_1        ((volatile unsigned int*)(PBASE+0x0000B204))    // BCM2835 page 112-116

#define IS_CORE_TIMER_IRQ    (1 << 1)
#define IS_GPU_IRQ           (1 << 8)
#define IS_AUX_IRQ           (1 << 29)
 
#endif