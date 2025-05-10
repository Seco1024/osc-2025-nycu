#include "kernel/kmalloc.h"
#include "lib/io.h"

void *allocate(int size) {
    if (size > SLAB_ALLOC_MAX && size < MAX_ORDER_SIZE << PAGE_SHIFT) {
        return buddy_malloc(size);
    } else if (size >= MAX_ORDER_SIZE << PAGE_SHIFT) {
        // puts("The number of pages required exceeds max order\n");
        return NULL;
    }
    for (int i = 0; i < SLAB_CACHE_COUNT; i++) {
        if (slab_caches[i].obj_size >= size) {
            return slab_malloc(&slab_caches[i]);
        }
    }
    return NULL;
}

void free(void *ptr) {
    if (ptr == NULL) {
        return ;
    }
    if ((uintptr_t)ptr % PAGE_SIZE == 0) {
        buddy_free(ptr);
    } else {
        slab_free(ptr);
    }
}