#ifndef _SLAB_H
#define _SLAB_H

#include "kernel/buddy.h"
#include "kernel/mm.h"
#include "lib/list.h"
#include "lib/stdlib.h"

#define SLAB_CACHE_COUNT 5
#define SLAB_SHIFT 4
#define ALIGN8(x)  (((x) + 7) & ~7)

typedef struct slab_cache {
    int obj_size;
    int num_objs;
    struct list_head partial;
    struct list_head full;
} slab_cache_t;

typedef struct slab {
    struct list_head list;
    int free_count;
    int num_objs;
    void *free_list; 
    slab_cache_t *cache;
} slab_t;

extern slab_cache_t slab_caches[SLAB_CACHE_COUNT];
void slab_init(void);
void slab_cache_init(slab_cache_t *, int);
void *slab_malloc(slab_cache_t *);
void slab_free(void *);

#endif