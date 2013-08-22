#include <page_alloc.h>

#include <stddef.h>
#include <svc.h>

#define MAX_ADDRESS 0x10000000
#define MAX_PAGES (MAX_ADDRESS/PAGE_SIZE)

#define MAX_LEVEL 8

#define PAGE_CLEAN 0
#define PAGE_DIRTY 1

extern unsigned end_kernel;

static unsigned char page_status[2*MAX_PAGES];

static int pages_on_level(unsigned level) {
	return MAX_PAGES >> level;
}

static unsigned char *base_for_level(unsigned level) {
	unsigned char *base = page_status;
	for (unsigned i = 0; i < level; ++i) {
		base += pages_on_level(i);
	}
	return base;
}

static size_t entry_on_level(size_t page, unsigned level) {
	return page >> level;
}

static void mark_dirty(const size_t page) {
	unsigned char *base = page_status;

	for (unsigned level = 0; level <= MAX_LEVEL &&
			base[entry_on_level(page, level)] == PAGE_CLEAN; ++level) {
		base[entry_on_level(page, level)] = PAGE_DIRTY;
		base += pages_on_level(level);
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

static size_t addr_to_page(void *addr) {
	return (size_t)addr / PAGE_SIZE;
}

extern char _binary_ramdisk_tar_start;
extern char _binary_ramdisk_tar_end;
void init_page_alloc(void) {
	unsigned kernel_end = (unsigned)&end_kernel;
	for (size_t i = 0; i <= kernel_end/PAGE_SIZE; ++i) {
		mark_dirty(i);
	}

	size_t ramdisk_first = addr_to_page(&_binary_ramdisk_tar_start);
	size_t ramdisk_end = addr_to_page(&_binary_ramdisk_tar_end);
	for (size_t page = ramdisk_first; page <= ramdisk_end; ++page) {
		mark_dirty(page);
	}

}

void *page_alloc(int level) {
	unsigned char *base = base_for_level(level);
	int entries_on_level = MAX_PAGES >> level;
	for (int i = 0; i < entries_on_level; ++i) {
		if (base[i] == PAGE_CLEAN) {
			mark_dirty_in_level(level, i);
			return (void*)((i << level)*PAGE_SIZE);
		}
	}
	return NULL;
}

void handle_page_alloc(unsigned *stack) {
	stack[r0] = (unsigned)page_alloc(stack[r0]);
}

/* TODO: Refactor this method */
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

void handle_page_free(unsigned *stack) {
	page_free((void*)stack[r0], stack[r1]);
}
