#include "kernel/slab.h"
#include "lib/io.h"

slab_cache_t slab_caches[SLAB_CACHE_COUNT];

void slab_cache_init(slab_cache_t *cache, int obj_size) {
    cache->obj_size = obj_size;
    cache->num_objs = 0;
    INIT_LIST_HEAD(&cache->full);
    INIT_LIST_HEAD(&cache->partial);
}

void slab_init(void) {
    for (int i = 0; i < SLAB_CACHE_COUNT; ++i) {
        int obj_size = 1 << (i + SLAB_SHIFT);
        slab_cache_init(&slab_caches[i], obj_size);
    }
}

static slab_t *slab_create(slab_cache_t *cache) {
    void *page = buddy_malloc(PAGE_SIZE);
    slab_t *slab = (slab_t *)page;
    INIT_LIST_HEAD(&slab->list);
    slab->cache = cache;

    // Calculate usable chunks: from (page + sizeof(slab_t)) to (page + PAGE_SIZE) 
    unsigned int header_size = sizeof(slab_t);
    unsigned int header_size_aligned = ALIGN8(header_size);
    unsigned int usable = PAGE_SIZE - header_size_aligned;
    slab->num_objs = usable / cache->obj_size;
    slab->free_count = usable / cache->obj_size;
    slab->free_list = NULL;

    // Construct a free list which start from (page + sizeof(slab_t))
    char *obj_ptr = (char *)page + header_size_aligned;
    for (int i = slab->num_objs - 1; i >= 0; i--) {
        void **chunk = (void **)(obj_ptr + i * cache->obj_size);
        *chunk = slab->free_list;
        slab->free_list = (void *)chunk;
    }

    return slab;
}

void *slab_malloc(slab_cache_t *cache) {
    slab_t *slab = NULL;
    if (!list_empty(&cache->partial)) {
        slab = list_first_entry(&cache->partial, slab_t, list);
    } else {
        slab = slab_create(cache);
        list_add(&slab->list, &cache->partial);
        cache->num_objs = slab->num_objs;
    }

    void *obj = slab->free_list;

    // Update free_list & free_count
    slab->free_list = *((void **)obj);
    slab->free_count--;

    if (slab->free_count == 0) {
        list_del(&slab->list);
        list_add(&slab->list, &cache->full);
    }

    #ifdef DEMO
        puts("[Chunk] Allocate ");
        puthex64((uintptr_t)obj);
        puts(" at chunk size ");
        puti(cache->obj_size);
        puts("\n");
    #endif

    return (void *)(uintptr_t)obj;
}

void slab_free(void *ptr) {
    if (!ptr) return ;
    uintptr_t address = (uintptr_t)ptr;
    uintptr_t page_address = address & ~(PAGE_SIZE - 1);
    slab_t *slab = (slab_t *)page_address;
    slab_cache_t *cache = slab->cache;
    *((void **)ptr) = slab->free_list;
    slab->free_list = ptr;
    slab->free_count++;

    if (slab->free_count == 1) {
        if (!list_empty(&cache->full)) {
            if (slab->list.next != &slab->list && slab->list.prev != &slab->list) {
                list_del(&slab->list);
                list_add(&slab->list, &cache->partial);
            }
        }
    }

    #ifdef DEMO
        puts("[Chunk] Free ");
        puthex64((uintptr_t)ptr);
        puts(" at chunk size ");
        puti(cache->obj_size);
        puts("\n");
    #endif

}