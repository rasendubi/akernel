#ifndef SLAB_H
#define SLAB_H

#include <stddef.h>

typedef struct slab_t slab_t;

/** Slab cache holds all the slabs of similar size. */
typedef struct slab_cache {
    slab_t *empty;
    slab_t *partial;
    slab_t *full;
    size_t size;
} slab_cache;

void slab_cache_init(slab_cache *cache, size_t object_size);
void slab_cache_delete(slab_cache *cache);

void *slab_cache_alloc(slab_cache *cache);
int slab_cache_free(slab_cache *cache, void *addr);

int in_this_slab_cache(slab_cache *cache, void *addr);

void slab_cache_mem_dump(slab_cache *cache);

#endif /* SLAB_H */
