#ifndef _KMALLOC_H
#define _KMALLOC_H

#include "lib/stdlib.h"
#include "kernel/mm.h"
#include "kernel/buddy.h"
#include "kernel/slab.h"

#define SLAB_ALLOC_MAX 256

void *allocate(int);
void free(void *);

#endif