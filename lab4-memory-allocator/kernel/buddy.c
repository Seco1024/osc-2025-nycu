#include "kernel/buddy.h"
#include "lib/io.h"

page_t *page_pool;
free_area_t free_area[MAX_ORDER + 1];

static inline int get_page_index(page_t *p) {
    return (int)(p - page_pool);
}

static inline int get_buddy_index(page_t *p, int order) {
    return (int)(p - page_pool) ^ (1 << order);
}

void push_to_free_area(page_t *page, free_area_t *free_area, int addToTail) {
    if (addToTail)
        list_add_tail(&page->list, &free_area->free_list);
    else
        list_add(&page->list, &free_area->free_list);
    free_area->nr_free++;

    // #ifdef DEMO
    //     puts("[+] Add page ");
    //     puti(get_page_index(page));
    //     puts(" to order ");
    //     puti(page->order);
    //     puts(".           Range of pages:[");
    //     puti(get_page_index(page));
    //     puts(", ");
    //     puti(get_page_index(page) + (1 << page->order) - 1);
    //     puts("]\n");
    // #endif

}

page_t *pop_from_free_area(free_area_t *free_area) {
    page_t *page = list_first_entry(&free_area->free_list, page_t, list);

    // #ifdef DEMO
    //     puts("[-] Remove page ");
    //     puti(get_page_index(page));
    //     puts(" from order ");
    //     puti(page->order);
    //     puts(".           Range of pages:[");
    //     puti(get_page_index(page));
    //     puts(", ");
    //     puti(get_page_index(page) + (1 << page->order) - 1);
    //     puts("]\n");
    // #endif

    list_del(&page->list);
    free_area->nr_free--;
    return page;
}

void remove_from_free_area(page_t *page, free_area_t *free_area) {

    // #ifdef DEMO
    //     puts("[-] Remove page ");
    //     puti(get_page_index(page));
    //     puts(" from order ");
    //     puti(page->order);
    //     puts(".           Range of pages:[");
    //     puti(get_page_index(page));
    //     puts(", ");
    //     puti(get_page_index(page) + (1 << page->order) - 1);
    //     puts("]\n");
    // #endif

    list_del(&page->list);
    free_area->nr_free--;
}

void buddy_init(uintptr_t start, uintptr_t end) {
    int num_pages = (end - start) >> PAGE_SHIFT;
    page_pool = (page_t *)simple_alloc(num_pages * sizeof(page_t));   // Startup Allocator

    for (int i = 0; i <= MAX_ORDER; ++i) {
        INIT_LIST_HEAD(&free_area[i].free_list);
        free_area[i].nr_free = 0;
    }

    for (int i = 0; i < num_pages; ++i) {
        INIT_LIST_HEAD(&page_pool[i].list);
        page_pool[i].order = -1;
        page_pool[i].status = PAGE_FREE;
    }

    for (int i = 0; i < num_pages; i += MAX_ORDER_SIZE) {
        page_pool[i].order = MAX_ORDER;
        page_pool[i].status = PAGE_FREE;
        push_to_free_area(&page_pool[i], &free_area[MAX_ORDER], ADD_TO_TAIL);
    }
}

page_t * __allocate_pages(int order) {
    int current_order;
    page_t *target = NULL;

    for (current_order = order; current_order <= MAX_ORDER; ++current_order) {
        if (!list_empty(&free_area[current_order].free_list)) {
            target = pop_from_free_area(&free_area[current_order]);
            break;
        }
    }

    if (!target) {
        puts("[Buddy] Failed to allocate page!");
        return NULL;
    }

    while (current_order > order) {
        current_order--;
        int buddy_id = get_buddy_index(target, current_order);
        page_t *buddy = &page_pool[buddy_id];
        buddy->order = current_order;
        buddy->status = PAGE_FREE;
        push_to_free_area(buddy, &free_area[current_order], ADD_TO_TAIL);
    }

    INIT_LIST_HEAD(&target->list);
    target->order = order;
    target->status = PAGE_USED;
    return target;
}

void __free_pages(page_t * target) {
    int current_order = target->order;
    while (current_order < MAX_ORDER) {
        int buddy_id = get_buddy_index(target, current_order);
        page_t *buddy = &page_pool[buddy_id];

        if (buddy->order != current_order || buddy->status == PAGE_USED)  
            break;
        
        remove_from_free_area(buddy, &free_area[current_order]);
        target = get_page_index(target) < get_page_index(buddy) ? target : buddy;
        current_order++;
    }
    target->status = PAGE_FREE;
    target->order = current_order;
    push_to_free_area(target, &free_area[current_order], NO_ADD_TO_TAIL);
}

void *buddy_malloc(uint32_t size) {
    uint32_t n_pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    int order = 0;
    while ((1 << order) < n_pages) {
        order++;
        if (order > 10) {
            puts("\n The number of pages required exceeds 2 ^ MAX_ORDER\n");
            return NULL;
        }
    } 
    
    page_t *page = __allocate_pages(order);
    if (!page) return NULL;
    uintptr_t target_address = (uintptr_t) ((get_page_index(page) << PAGE_SHIFT) + USER_MEM_BASE);
    
    #ifdef DEMO
        puts("[Page] Allocate ");
        puthex64(target_address);
        puts(" at order ");
        puti(page->order);
        puts(", page ");
        puti(get_page_index(page));
        puts("\n");
    #endif

    return (void *)target_address;
}

void buddy_free(void *address) {
    uintptr_t offset = (uintptr_t)address - USER_MEM_BASE;
    int page_idx = offset / PAGE_SIZE;
    page_t *page = &page_pool[page_idx];
    __free_pages(page);

    #ifdef DEMO
        puts("[Page] Free ");
        puthex64((uintptr_t)address);
        puts(" and add back to order ");
        puti(page->order);
        puts(", page ");
        puti(get_page_index(page));
        puts("\n");
    #endif

}

void memory_reserve(uintptr_t start, uintptr_t end) {
    start &= ~(PAGE_SIZE - 1);
    end = (end + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    for (int current_order = MAX_ORDER; current_order >= 0; current_order--) {
        struct list_head *pos, *n;
        list_for_each_safe(pos, n, &free_area[current_order].free_list) {
            page_t *page = list_entry(pos, page_t, list);
            uintptr_t page_address = get_page_index(page) << PAGE_SHIFT;
            uintptr_t block_size = (1UL << (current_order + PAGE_SHIFT));
            uintptr_t page_end = page_address + block_size - 1;

            if (current_order == 0) {
                if (!(page_end < start || page_address > end)) {
                    page->status = PAGE_RESERVED;
                    remove_from_free_area(page, &free_area[current_order]);

                    #ifdef DEMO
                        puts("[x] Reserve address [");
                        puthex64(page_address);
                        puts(", ");
                        puthex64(page_end);
                        puts(")           Range of pages:[");
                        puti(get_page_index(page));
                        puts(", ");
                        puti(get_page_index(page) + (1 << page->order));
                        puts(")\n");
                    #endif

                }
            } else if (page_address >= start && page_end <= end) {
                page->status = PAGE_RESERVED;
                remove_from_free_area(page, &free_area[current_order]);

                #ifdef DEMO
                    puts("[x] Reserve address [");
                    puthex64(page_address);
                    puts(", ");
                    puthex64(page_end);
                    puts(")           Range of pages:[");
                    puti(get_page_index(page));
                    puts(", ");
                    puti(get_page_index(page) + (1 << page->order));
                    puts(")\n");
                #endif

            } else if (page_end < start || page_address > end) {
            } else {
                int buddy_id = get_buddy_index(page, current_order - 1);
                page_t *buddy = &page_pool[buddy_id];

                remove_from_free_area(page, &free_area[current_order]);
                page->order = current_order - 1;
                buddy->order = current_order - 1;
                push_to_free_area(buddy, &free_area[current_order - 1], ADD_TO_TAIL);
                push_to_free_area(page, &free_area[current_order - 1], ADD_TO_TAIL);
            }
        }
    }
}