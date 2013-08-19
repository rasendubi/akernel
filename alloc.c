#include <alloc.h>

#include <page_alloc.h>

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

void *malloc(size_t size) {
	if (size == 0) {
		return NULL;
	}
	size = max(size + sizeof(occupied_struct),
			sizeof(free_struct));
	free_struct *prev = get_free_for(size);
	if (!prev->next) {
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
	return (void *)((unsigned)occ + sizeof(*occ));
}

void free(void *addr) {
	if (addr == 0) {
		return;
	}

	occupied_struct *occ = (occupied_struct*)((unsigned)addr -
			sizeof(struct occupied_struct));
	size_t size = occ->size;
	free_struct *free_region = (free_struct *)occ;
	free_region->size = size;
	add_free_region(free_region);
}
