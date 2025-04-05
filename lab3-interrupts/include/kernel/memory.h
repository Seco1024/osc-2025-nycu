#ifndef __MEMORY_H
#define __MEMORY_H

#define USER_PROGRAM_BASE ((char *)0x100000)
#define USER_STACK_POINTER_BASE ((char *)0x200000)

void memory_init(void);
void* simple_alloc(unsigned int);
void test_simple_alloc(int, char **);

#endif