#include <gic.h>
#include <page_alloc.h>
#include <pipe.h>
#include <print.h>
#include <scheduler.h>
#include <user.h>
#include <user/syscalls.h>

unsigned hyp_stack[2048];

void init_systems(void) {
	if (!fork()) {
		do {
			sys_exec("user/services/pipe_master");
			printa("pipe_master error\n");
		} while (1);
	}
	if (!fork())
		user_first();

	while (1);
}

int main(void) {
	printa("In main %x\n", (unsigned)main);
	init_int();
	init_page_alloc();
	init_pipes();
	init_scheduler();

	add_task(&init_systems);

	while (1) {
		schedule();
	}

	return 0;
}
