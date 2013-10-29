#include <alloc.h>
#include <print.h>
#include <uart.h>
#include <user_pipe_master.h>
#include <user/syscalls.h>

static int ticks;
static int user_isr(unsigned line) {
	(void)line;
	if (++ticks%8 == 0) {
		printa("%x ticks\n", ticks);
	}
	return 0;
}

void _start(void) {
	ticks = 0;
	sys_register_isr(36, &user_isr);
	while (1);
}
