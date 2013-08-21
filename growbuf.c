#include <growbuf.h>

#include <alloc.h>
#include <page_alloc.h>

typedef struct grow_page {
	char *page;
	struct grow_page *next;
} grow_page;

static inline grow_page *grow_page_new() {
	grow_page *res = malloc(sizeof(grow_page));
	res->page = page_alloc(0);
	res->next = 0;
	return res;
}

static inline void grow_page_free(grow_page *gp) {
	page_free(gp->page, 0);
	free(gp);
}

struct growbuf {
	struct grow_page *first_page;
	struct grow_page *last_page;
	int first_pos, last_pos;
	int n_pages;
};

growbuf *growbuf_new() {
	growbuf *res = malloc(sizeof(growbuf));
	res->first_page = res->last_page = 0;
	res->first_pos = 0;
	res->last_pos = -1;
	res->n_pages = 0;
	return res;
}

void growbuf_free(growbuf *gb) {
	grow_page *cur = gb->first_page;
	while (cur) {
		grow_page *tmp = cur->next;
		grow_page_free(cur);
		cur = tmp;
	}
	free(gb);
}

char grow_pop(growbuf *gb) {
	if (!gb->n_pages)
		return 0;

	char c = gb->first_page->page[gb->first_pos];
	if (!(gb->first_pos = (gb->first_pos + 1) % PAGE_SIZE)) {
		--gb->n_pages;
		grow_page *tmp = gb->first_page;
		gb->first_page = gb->first_page->next;
		grow_page_free(tmp);
	}
	return c;
}

void grow_push(growbuf *gb, char c) {
	if (!(gb->last_pos = (gb->last_pos + 1) % PAGE_SIZE)) {
		++gb->n_pages;
		grow_page *tmp = grow_page_new();
		if (!gb->last_page) {
			gb->first_page = gb->last_page = tmp;
		} else {
			gb->last_page->next = tmp;
			gb->last_page = tmp;
		}
	}

	gb->last_page->page[gb->last_pos] = c;
}

size_t grow_len(growbuf *gb) {
	return gb->last_pos == -1 ? 0 : gb->n_pages*PAGE_SIZE - gb->first_pos -
		(PAGE_SIZE - gb->last_pos - 1);
}
