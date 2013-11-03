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
	printa("In init systems\n");

	if (!sys_fork()) {
		printa("Starting user/user_first\n");
		sys_exec("user/user_first");
		sys_exit(0);
	}

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

	schedule();

	return 0;
}
