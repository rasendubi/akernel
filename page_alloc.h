#ifndef PAGE_ALLOC_H
#define PAGE_ALLOC_H

#define PAGE_SIZE 4096

void init_page_alloc(void);

void *page_alloc(int level);
void page_free(void *page_start, int level);

#endif /* PAGE_ALLOC_H */
