#include <uart.h>
#include <asm.h>

void user_first(void) {
	uart_puts("In user mode\n");
	while(1);
}

int main(void) {
	uart_puts("Hello, world!\n");
	activate();

	while (1);
	return 0;
}

