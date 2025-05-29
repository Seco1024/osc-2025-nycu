#include "kernel/mm.h"
#include "lib/stdlib.h"
#include "lib/io.h"

extern char __bss_end[];
static uintptr_t heap_top;

void memory_init(void) {
    heap_top = (uintptr_t)&__bss_end;
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