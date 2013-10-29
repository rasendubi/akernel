#include <user.h>

#include <alloc.h>
#include <print.h>
#include <uart.h>
#include <user_pipe_master.h>
#include <utils.h>
#include <user/syscalls.h>

static void irq_test(void);

static void task(void) {
	const char *message = "Hello from task\n";
	int pipe = pipe_open("/task1");
	char *buf = malloc(sizeof(unsigned) + strlen(message) + 1);
	((unsigned*)buf)[0] = strlen(message) + 1;
	memcpy(buf + sizeof(unsigned), message, strlen(message) + 1);
	write(pipe, buf, sizeof(unsigned) + strlen(message) + 1);
	printa("In other task\n");
	while (1);
}

void user_first(void) {
	int pipe = pipe_new("/task1");
	printa("In user mode\n");
	if (!fork()) irq_test();
	if (!fork()) {
		sys_exec("stupid");
		printa("stupid returned\n");
		while (1);
	}
	if (!fork()) sys_exec("user/alloc_test");
	if (!fork()) task();
	if (!fork()) sys_exec("user/print_test");
	printa("In user mode again\n");
	while (1) {
		int len;
		read(pipe, &len, sizeof(len));
		char *buf = malloc(len);
		read(pipe, buf, len);
		printa("Message read: %s", buf);
		free(buf);
	}
}

static int ticks;
static int user_isr(unsigned line) {
	(void)line;
	if (++ticks%8 == 0) {
		printa("%x ticks\n", ticks);
	}
	return 0;
}

static void irq_test(void) {
	ticks = 0;
	sys_register_isr(36, &user_isr);
	while (1);
}
