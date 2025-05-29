#ifndef GPIO_H
#define GPIO_H

#include "kernel/mmu.h"

#define PBASE   VIRT_OFFSET+0x3F000000

#define GPFSEL1         ((volatile unsigned int*)(PBASE+0x00200004))
#define GPPUD           ((volatile unsigned int*)(PBASE+0x00200094))
#define GPPUDCLK0       ((volatile unsigned int*)(PBASE+0x00200098))


#endif