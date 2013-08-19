#include <page_alloc.h>

#include <stddef.h>

#define MAX_ADDRESS 0x10000000
#define MAX_PAGES (MAX_ADDRESS/PAGE_SIZE)

#define MAX_LEVEL 8

#define PAGE_CLEAN 0
#define PAGE_DIRTY 1

extern unsigned end_kernel;

static unsigned char page_status[2*MAX_PAGES];

static void mark_dirty(const size_t page) {
	size_t entry = page;
	unsigned level = 0;
	unsigned char *base = page_status;

	while (level <= MAX_LEVEL && base[entry] == PAGE_CLEAN) {
		base[entry] = PAGE_DIRTY;
		base += MAX_PAGES >> level;
		++level;
	}
}

static void mark_dirty_in_level(int level, int entry) {
	const int first_page = entry << level;
	const int end_page = first_page + (1<<level);
	for (int i = first_page; i < end_page; ++i) {
		mark_dirty(i);
	}
}

void init_page_alloc(void) {
	unsigned kernel_end = (unsigned)&end_kernel;
	for (size_t i = 0; i < kernel_end/PAGE_SIZE; ++i) {
		mark_dirty(i);
	}
}

void *page_alloc(int level) {
	unsigned char *base = page_status;
	int level_entries = MAX_PAGES >> level;
	for (int i = 0; i < level; ++i) {
		base += MAX_PAGES >> i;
	}
	for (int i = 0; i < level_entries; ++i) {
		if (base[i] == PAGE_CLEAN) {
			mark_dirty_in_level(level, i);
			return (void*)((i << level)*PAGE_SIZE);
		}
	}
	return NULL;
}

void page_free(void *page_start, int level) {
	unsigned char *base = page_status;
	int entry_on_level = ((unsigned)page_start/PAGE_SIZE) >> level;
	int big_brother = entry_on_level - entry_on_level%2;
	for (int cur_level = 0; cur_level < level; ++cur_level) {
		int entry_count = 1 << (level - cur_level);
		int start_entry = ((unsigned)page_start/PAGE_SIZE) >> cur_level;
		for (int i = 0; i < entry_count; ++i) {
			base[start_entry + i] = PAGE_CLEAN;
		}
		base += MAX_PAGES >> cur_level;
	}
	base[entry_on_level] = PAGE_CLEAN;
	if (base[big_brother] == PAGE_CLEAN && base[big_brother+1] ==
			PAGE_CLEAN) {
		base += MAX_PAGES >> level;
		base[entry_on_level/2] = PAGE_CLEAN;
	}
}

