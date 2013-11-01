#include <alloc.h>
#include <print.h>
#include <uart.h>
#include <user_pipe_master.h>
#include <utils.h>
#include <user/syscalls.h>

static void task(void) {
	const char *message = "Hello from task\n";
	int pipe = pipe_open("/task1");
	char *buf = malloc(sizeof(unsigned) + strlen(message) + 1);
	((unsigned*)buf)[0] = strlen(message) + 1;
	memcpy(buf + sizeof(unsigned), message, strlen(message) + 1);
	sys_write(pipe, buf, sizeof(unsigned) + strlen(message) + 1);
	printa("In other task\n");
	sys_exit(0);
	printa("Exit failed\n");
}

void _start(void) {
	int pipe = pipe_new("/task1");
	printa("In user mode\n");
	if (!sys_fork()) sys_exec("user/irq_test");
	if (!sys_fork()) {
		sys_exec("user/stupid");
		printa("stupid returned\n");
		while (1);
	}
	if (!sys_fork()) sys_exec("user/alloc_test");
	if (!sys_fork()) task();
	if (!sys_fork()) sys_exec("user/print_test");
	printa("In user mode again\n");
	while (1) {
		int len;
		sys_read(pipe, &len, sizeof(len));
		char *buf = malloc(len);
		sys_read(pipe, buf, len);
		printa("Message read: %s", buf);
		free(buf);
	}
}
