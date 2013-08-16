#include <user.h>

#include <asm.h>
#include <uart.h>

static void task(void) {
	char message[200] = "Hello from task\n";
	uart_puts("In other task\n");
	write(0, message, 100);
	while (1);
}

void user_first(void) {
	char buf[200];
	uart_puts("In user mode\n");
	if (!fork()) task();
	uart_puts("In user mode again\n");
	read(0, buf, 100);
	uart_puts("Message read: ");
	uart_puts(buf);
	while (1);
}

