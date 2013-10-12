#ifndef SLAB_ALLOC_H
#define SLAB_ALLOC_H

#include <stddef.h>

void *slab_alloc(size_t size);
void slab_free(void *addr);
void *slab_realloc(void *addr, size_t size);

void slab_mem_dump(void);

#endif /* SLAB_ALLOC_H */
