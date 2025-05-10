#ifndef _BUDDY_H
#define _BUDDY_H

#include "lib/list.h"
#include "lib/stdlib.h"
#include "kernel/mm.h"

#define MAX_ORDER 10 
#define MAX_ORDER_SIZE (1 << MAX_ORDER)

#define PAGE_FREE  0
#define PAGE_USED  1
#define PAGE_RESERVED  2

#define ADD_TO_TAIL 1
#define NO_ADD_TO_TAIL 0 

typedef struct page {
    struct list_head list;
    int order;
    uint8_t status;
} page_t;

typedef struct free_area {
    struct list_head free_list;
    int nr_free;
} free_area_t;

extern page_t *page_pool;
extern free_area_t free_area[MAX_ORDER + 1];

void buddy_init(uintptr_t, uintptr_t);
void memory_reserve(uintptr_t, uintptr_t);
void *buddy_malloc(uint32_t);
void buddy_free(void *);

page_t *__allocate_pages(int);
void __free_pages(page_t *);

#endif