#include <slab.h>

#include <print.h>
#include <uart.h>

#include <page_alloc.h>

/**
 * Slab object. All slabs are located in one-way linked lists.
 */
struct slab_t {
    /** Start of allocated page. */
    void *start;
    /** Next slab. */
    struct slab_t *next;
    /** Number of allocated slots. */
    int allocated;
    /** Bitarray whether slot is allocated. */
    unsigned char bitarray[];
};

static inline void bitarray_set(unsigned char array[], unsigned index, unsigned value) {
    array[index/8] = (array[index/8] & ~(1 << index%8)) | (!!value << index%8);
}

static inline int bitarray_get(unsigned char array[], unsigned index) {
    return !!(array[index/8] & (1 << index%8));
}

/** Calculates maximal amount of object slots fit in page with respect to
 * slab header at and of page.
 */
static inline int max_objects_for_size(size_t size) {
    return (PAGE_SIZE - sizeof(slab_t))*8 / (size*8+1);

}

/**
 * Creates new slab. New page from system is allocated.
 */
static slab_t *slab_new(size_t size) {
    void * const start = page_alloc(0);
    const int max_objects = max_objects_for_size(size);
    slab_t *slab = (slab_t *)((unsigned)start + max_objects*size);
    slab->start = start;
    slab->next = NULL;
    for (int i = 0; i < max_objects; ++i) {
        bitarray_set(slab->bitarray, i, 0);
    }
    slab->allocated = 0;
    return slab;
}

void slab_cache_init(slab_cache *cache, size_t object_size) {
    cache->size = object_size;
    cache->empty = cache->partial = cache->full = NULL;
}

static void slab_list_free(slab_t *slab) {
    slab_t *cur = slab;
    while (cur) {
        slab_t *next = cur->next;
        page_free(slab->start, 0);
        cur = next;
    }
}

void slab_cache_delete(slab_cache *cache) {
    slab_list_free(cache->empty);
    slab_list_free(cache->partial);
    slab_list_free(cache->full);
}

static void *alloc_in_slab(slab_t *slab, size_t size) {
    const int max_objects = max_objects_for_size(size);
    for (int i = 0; i < max_objects; ++i) {
        if (bitarray_get(slab->bitarray, i) == 0) {
            ++slab->allocated;
            bitarray_set(slab->bitarray, i, 1);
            return (void *)((unsigned)slab->start + i*size);
        }
    }
    return NULL;
}

static void free_in_slab(slab_t *slab, void *addr, size_t size) {
    int index = (unsigned)addr % PAGE_SIZE / size;
    bitarray_set(slab->bitarray, index, 0);
    --slab->allocated;
}

void *slab_cache_alloc(slab_cache *cache) {
    if (!cache->partial) {
        slab_t *slab;
        if (cache->empty) {
            slab = cache->empty;
            cache->empty = slab->next;
        } else {
            slab = slab_new(cache->size);
        }
        slab->next = NULL;
        cache->partial = slab;
    }

    void *result = alloc_in_slab(cache->partial, cache->size);
    if (cache->partial->allocated == max_objects_for_size(cache->size)) {
        slab_t *slab = cache->partial;
        cache->partial = cache->partial->next;
        slab->next = cache->full;
        cache->full = slab;
    }
    return result;
}

static int in_this_slab(slab_t *slab, void *addr) {
    return (unsigned)slab/PAGE_SIZE == (unsigned)addr/PAGE_SIZE;
}

/** Tries to free object in every slab. Returns slab on success, NULL otherwise. */
static slab_t *free_in_slab_list(slab_t *head, void *addr, size_t size, slab_t **prev_out) {
    slab_t *prev = NULL;
    slab_t *cur = head;
    while (cur) {
        if (in_this_slab(cur, addr)) {
            free_in_slab(cur, addr, size);
            if (prev_out) {
                *prev_out = prev;
            }
            return cur;
        }
        prev = cur;
        cur = cur->next;
    }
    return NULL;
}

int slab_cache_free(slab_cache *cache, void *addr) {
    slab_t *prev = NULL;

    slab_t *cur = free_in_slab_list(cache->full, addr, cache->size, &prev);
    if (cur) {
        if (prev) {
            prev->next = cur->next;
        } else {
            cache->full = cur->next;
        }
        cur->next = cache->partial;
        cache->partial = cur;
        return 1;
    }

    cur = free_in_slab_list(cache->partial, addr, cache->size, &prev);
    if (cur) {
        if (!cur->allocated) {
            if (prev) {
                prev->next = cur->next;
            } else {
                cache->partial = cur->next;
            }
            cur->next = cache->empty;
            cache->empty = cur;
        }
        return 1;
    }

    return 0;
}

static int in_this_slab_list(slab_t *head, void *addr) {
    for (slab_t *cur = head; cur; cur = cur->next) {
        if (in_this_slab(cur, addr)) {
            return 1;
        }
    }
    return 0;
}

int in_this_slab_cache(slab_cache *cache, void *addr) {
    return in_this_slab_list(cache->full, addr) || in_this_slab_list(cache->partial, addr);
}

static void dump_slab_list(slab_t *head, size_t size) {
    slab_t *cur = head;
    while (cur) {
        printa("slab (allocated %x)\n", cur->allocated);
        int max_objects = max_objects_for_size(size);
        for (int i = 0; i < max_objects; ++i) {
            uart_putchar('0' + bitarray_get(cur->bitarray, i));
        }
        uart_putchar('\n');
        cur = cur->next;
    }
}

void slab_cache_mem_dump(slab_cache *cache) {
    printa("Slab cache size: %x\n", cache->size);
    printa("Empty list\n");
    dump_slab_list(cache->empty, cache->size);
    printa("----------\nPartial list\n");
    dump_slab_list(cache->partial, cache->size);
    printa("----------\nFull list\n");
    dump_slab_list(cache->full, cache->size);
    printa("----------\n");
}
