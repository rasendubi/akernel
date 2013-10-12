#include <slab_alloc.h>

#include <slab.h>
#include <page_alloc.h>
#include <utils.h>

#define MAX_POWER 10
#define MAX_SIZE (1 << MAX_POWER)

static int initialized = 0;
static slab_cache caches[MAX_POWER];

typedef struct big_object {
    void *start;
    size_t page_power;
    struct big_object *next;
} big_object;

slab_cache big_object_cache = { .size = sizeof(big_object) };

big_object first_big = { .next = NULL };

static inline void init_slab_allocator() {
    if (!initialized) {
        for (int i = 0; i < MAX_POWER; ++i) {
            slab_cache_init(&caches[i], 1 << i);
        }
    }
    initialized = 1;
}

static inline int get_2_power(size_t size) {
    int power = 0;
    while (size) {
        ++power;
        size /= 2;
    }
    return power;
}

void *slab_alloc(size_t size) {
    init_slab_allocator();

    if (size == 0) {
        return NULL;
    }

    if (size > MAX_SIZE) {
        big_object *bo = slab_cache_alloc(&big_object_cache);
        bo->next = first_big.next;
        first_big.next = bo;

        bo->page_power = get_2_power(size/PAGE_SIZE);
        bo->start = page_alloc(bo->page_power);
        return bo->start;
    }

    int power_of_2 = get_2_power(size-1);
    printa("Allocate %x in %x\n", size, power_of_2);
    return slab_cache_alloc(&caches[power_of_2]);
}

void slab_free(void *addr) {
    if (!addr) {
        return;
    }

    big_object *bo = &first_big;
    while (bo->next) {
        if (bo->next->start == addr) {
            big_object *tmp = bo->next;
            page_free(tmp->start, tmp->page_power);
            bo->next = tmp->next;
            slab_cache_free(&big_object_cache, tmp);
            return;
        }
        bo = bo->next;
    }

    for (int i = 0; i < MAX_POWER; ++i) {
        if (slab_cache_free(&caches[i], addr)) {
            return;
        }
    }
}

void *slab_realloc(void *addr, size_t size) {
    if (!addr) {
        return slab_alloc(size);
    }
    if (!size) {
        slab_free(addr);
    }

    big_object *bo = &first_big;
    while (bo->next) {
        if (bo->next->start == addr) {
            size_t object_size = (1 << bo->next->page_power)*PAGE_SIZE;
            if (object_size >= size) {
                return addr;
            }
            void *new_pos = slab_alloc(size);
            memcpy(new_pos, addr, object_size);

            big_object *tmp = bo->next;
            bo->next = tmp->next;
            page_free(tmp->start, tmp->page_power);
            slab_cache_free(&big_object_cache, bo);
            return new_pos;
        }
        bo = bo->next;
    }

    for (int i = MAX_POWER - 1; i >= 0; --i) {
        if (in_this_slab_cache(&caches[i], addr)) {
            size_t slot_size = 1 << i;
            if (slot_size >= size) {
                return addr;
            }

            void *new_pos = slab_alloc(size);
            memcpy(new_pos, addr, slot_size);
            slab_cache_free(&caches[i], addr);
            return new_pos;
        }
    }
    return NULL;
}

void slab_mem_dump(void) {
    big_object *bo = first_big.next;
    while (bo) {
        printa("Big object at: %x (%x)\n", bo->start, bo->page_power);
        bo = bo->next;
    }

    for (int i = 0; i < MAX_POWER; ++i) {
        printa("Slab %x\n", i);
        slab_cache_mem_dump(&caches[i]);
    }
}
