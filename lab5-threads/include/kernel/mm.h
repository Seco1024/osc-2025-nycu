#ifndef __MM_H
#define __MM_H

#define USER_PROGRAM_BASE ((char *)0x100000)
#define USER_STACK_POINTER_BASE ((char *)0x200000)
#define USER_MEM_BASE (0x0)

#define PAGE_SHIFT 12
#define PAGE_SIZE 4096

#define HEAP_SIZE 0x1000000
#define HEAP_MAX ((uintptr_t)&__bss_end + HEAP_SIZE)

extern char __bss_end[];

void memory_init(void);
void* simple_alloc(unsigned int);
void test_simple_alloc(int, char **);

#endif