#include <user.h>

#include <asm.h>
#include <uart.h>

static void task(void) {
	uart_puts("In other task\n");
	while (1);
}

void user_first(void) {
	uart_puts("In user mode\n");
	if (!fork()) task();
	uart_puts("In user mode again\n");
	while (1);
}

