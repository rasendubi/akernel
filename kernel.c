#include <uart.h>
#include <asm.h>

unsigned int first_stack[256];

void user_first(void) {
	uart_puts("In user mode\n");
	while(1);
}

int main(void) {
	uart_puts("Hello, world!\n");
	activate(first_stack + 256);

	while (1);
	return 0;
}

