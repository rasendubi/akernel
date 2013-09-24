#include <alloc.h>

#include <page_alloc.h>
#include <utils.h>

#define ALIGN 4

#ifdef DEBUG_ALLOC
	#include <print.h>
	#define debug(...) printa(__VA_ARGS__)
#else
	#define debug(...) do {} while(0)
#endif

typedef struct free_struct {
	size_t size;
	struct free_struct *next;
} free_struct;

typedef struct occupied_struct {
	size_t size;
} occupied_struct;

static inline size_t max(const size_t a, const size_t b) {
	return a > b ? a : b;
}

struct free_struct first_free = {
	.size = 0,
	.next = 0
};

static inline int get_2_power(size_t size) {
	int power = 0;
	size /= PAGE_SIZE;
	while (size) {
		++power;
		size /= 2;
	}
	return power;

}

static free_struct *add_free_region(free_struct *region) {
	free_struct *cur = &first_free;
	free_struct *prev = 0;
	while (cur->next && cur->next > region) {
		prev = cur;
		cur = cur->next;
	}
	if ((unsigned)cur+cur->size == (unsigned)region) {
		cur->size += region->size;
	} else {
		region->next = cur->next;
		cur->next = region;
		prev = cur;
		cur = region;
	}

	if ((unsigned)cur+cur->size == (unsigned)cur->next) {
		cur->size += cur->next->size;
		cur->next = cur->next->next;
	}
	return prev;
}

static free_struct *add_free_page(size_t size) {
	int power = get_2_power(size);
	size_t real_size = (1 << power)*PAGE_SIZE;
	free_struct *new_page = page_alloc(power);
	new_page->size = real_size;
	return add_free_region(new_page);
}

static free_struct *get_free_for(size_t size) {
	free_struct *cur = &first_free;
	while (cur->next && cur->next->size < size) {
		cur = cur->next;
	}
	if (!cur->next) {
		cur = add_free_page(size);
	}
	return cur;
}

static occupied_struct *get_from_next_region(free_struct *prev, size_t size) {
	if (prev->next->size < size) {
		return NULL;
	}

	occupied_struct *occ = (occupied_struct *)prev->next;

	size_t rest = prev->next->size - size;
	if (rest < sizeof(free_struct)) {
		size = prev->next->size;
		prev->next = prev->next->next;
	} else {
		free_struct *new_free = (free_struct *)((unsigned)occ + size);
		new_free->size = rest;
		new_free->next = prev->next->next;
		prev->next = new_free;
	}

	occ->size = size;
	return occ;
}

void *malloc(size_t size) {
	debug("Allocated ");
	if (size == 0) {
		debug("0\n");
		return NULL;
	}
	size = max(size + sizeof(occupied_struct),
			sizeof(free_struct));
	size = ((size - 1)/ALIGN + 1)*ALIGN;
	free_struct *prev = get_free_for(size);
	if (!prev->next) {
		return NULL;
	}
	occupied_struct *occ = get_from_next_region(prev, size);

	debug("%x-%x\n", (unsigned)occ, (unsigned)occ + size);
	return (void *)((unsigned)occ + sizeof(*occ));
}

void free(void *addr) {
	debug("Freed ");
	if (addr == 0) {
		debug("0\n");
		return;
	}

	occupied_struct *occ = (occupied_struct*)((unsigned)addr -
			sizeof(struct occupied_struct));
	size_t size = occ->size;
	free_struct *free_region = (free_struct *)occ;
	free_region->size = size;
	add_free_region(free_region);
	debug("%x-%x\n", occ, (unsigned)occ + size);
}

static occupied_struct *extend_realloc(occupied_struct *occ,
		size_t size) {
	void *next_after_cur = (void *)((unsigned)occ + occ->size);
	free_struct *fs = &first_free;
	free_struct *best_pre = fs;
	size_t extra = size - occ->size;
	while (fs->next) {
		if (fs->next == next_after_cur) {
			if (fs->next->size > extra) {
				size_t rest = fs->next->size - extra;
				if (rest > sizeof(free_struct)) {
					free_struct *new_free =
						(free_struct*)((unsigned)fs->next +
						extra);
					new_free->next = fs->next->next;
					new_free->size = rest;
					occ->size += extra;
				} else {
					occ->size += fs->next->size;
					fs->next = fs->next->next;
				}
				return occ;
			}
		} else if (fs->next->size >= size) {
			if (fs->next->size < best_pre->next->size) {
				best_pre = fs;
			}
		}
		fs = fs->next;
	}

	if (best_pre->next->size < size) {
		best_pre = add_free_page(size);
	}


	occupied_struct *os = get_from_next_region(best_pre, size);
	memcpy((void *)((unsigned)os + sizeof(*os)),
			(void *)((unsigned)occ + sizeof(*occ)),
			occ->size);
	os->size = size;
	return os;
}

void *realloc(void *old, size_t size_in) {
	if (old == NULL) {
		return malloc(size_in);
	}
	if (size_in == 0) {
		free(old);
		return NULL;
	}

	size_t size = size_in + sizeof(occupied_struct);
	size = ((size - 1)/ALIGN + 1)*ALIGN;

	occupied_struct *occ = (occupied_struct *)((size_t)old -
			sizeof(occupied_struct));
	if (size <= occ->size) {
		/* Shrink */
		size_t new_free_size = occ->size - size;
		if (new_free_size > sizeof(free_struct)) {
			occ->size -= new_free_size;
			free_struct *fs = (free_struct *)((unsigned)occ +
					(size - new_free_size));
			fs->size = new_free_size;
			add_free_region(fs);
		}
		return old;
	} else {
		occupied_struct *os = extend_realloc(occ, size);
		return (void *)((unsigned)os + sizeof(*os));
	}
}

void *calloc(size_t num, size_t elem_size) {
	size_t size = num*elem_size;
	void *mem = malloc(size);
	memset(mem, 0, size);
	return mem;
}
