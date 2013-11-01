#include <gic.h>
#include <page_alloc.h>
#include <pipe.h>
#include <print.h>
#include <scheduler.h>
#include <user/syscalls.h>

/* Define hypervisor stack */
unsigned hyp_stack[2048];

/* Initialize all services and execute user level code */
void init_systems(void) {
	if (!sys_fork()) {
		do {
			sys_exec("user/services/pipe_master");
			printa("pipe_master error\n");
		} while (1);
	}
	if (!sys_fork())
		sys_exec("user/user_first");

	while (1);
}

/* Initialize all kernel subsystems and run system */
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
