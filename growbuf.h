#ifndef GROWBUF_H
#define GROWBUF_H

#include <stddef.h>

struct growbuf;
typedef struct growbuf growbuf;

growbuf *growbuf_new();
void growbuf_free(growbuf *);

char grow_pop(growbuf *);
void grow_push(growbuf *, char);

size_t grow_len(growbuf *);

#endif /* GROWBUF_H */
