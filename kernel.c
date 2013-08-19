#include <gic.h>
#include <page_alloc.h>
#include <pipe.h>
#include <scheduler.h>
#include <user.h>

unsigned hyp_stack[2048];

int main(void) {
	init_int();
	init_page_alloc();
	init_pipes();
	init_scheduler();

	add_task(&user_first);

	while (1) {
		schedule();
	}

	return 0;
}
