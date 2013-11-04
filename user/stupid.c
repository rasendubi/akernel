#include <uart.h>
#include <user/syscalls.h>

volatile int glob = 10,
	 a = 5, b = 4;

void _start(void) {
	glob = a + b;
	uart_puts("Hello from stupid!!!!\n");
	sys_exit(0);
}

int first(void) {
	return glob = a + b;
}
