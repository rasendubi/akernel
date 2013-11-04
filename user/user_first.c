#include <alloc.h>
#include <print.h>
#include <uart.h>
#include <user_pipe_master.h>
#include <utils.h>
#include <user/syscalls.h>

void _start(void) {
	printa("In user mode\n");
	if (!sys_fork()) sys_exec("user/irq_test");
	if (!sys_fork()) sys_exec("user/stupid");
	if (!sys_fork()) sys_exec("user/alloc_test");
	if (!sys_fork()) sys_exec("user/print_test");
	printa("In user mode again\n");

	while (1);
}
