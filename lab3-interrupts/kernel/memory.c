#include "kernel/memory.h"
#include "lib/stdlib.h"
#include "lib/io.h"


extern int __bss_end;

#define HEAP_SIZE 0x1000 
#define HEAP_MAX ((uint64_t *)&__bss_end + HEAP_SIZE)

static uint64_t *heap_top;

void memory_init(void) {
    heap_top = (uint64_t *)&__bss_end;
}

void *simple_alloc(unsigned int size) {
    if (heap_top + size > HEAP_MAX) {
        return ((void *)0);
    }

    void *current_ptr = (void *)heap_top;
    heap_top += size;
    return current_ptr;
}

void test_simple_alloc(int argc, char **argv) {
    puts("Size to allocate: ");
    async_gets();
    unsigned int size = atoi(buffer);
    void *ptr = simple_alloc(size);

    puts("Malloc address: ");
    puthex((unsigned long)ptr);
    puts("\n");

    puts("Heap top address: ");
    puthex((unsigned long)heap_top);
    puts("\n");
}