#ifndef ALLOC_H
#define ALLOC_H

#include <stddef.h>

void *malloc(size_t size);
void free(void *addr);
void *realloc(void *old, size_t size);

#endif /* ALLOC_H */
