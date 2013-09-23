#include <page_alloc.h>

#include <uart.h>

#include <user/syscalls.h>

void *page_alloc(int level) {
	return sys_page_alloc(level);
}

void page_free(void *page_start, int level) {
	sys_page_free(page_start, level);
}
