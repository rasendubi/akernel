#include <uart.h>
#include <asm.h>

unsigned int first_stack[256];

void user_first(void) {
	uart_puts("In user mode\n");
	syscall();
	while(1);
}

int main(void) {

	unsigned int *first_stack_start = first_stack + 256 - 16;
	first_stack_start[0] = 0x10;
	first_stack_start[1] = (unsigned int)&user_first;
	uart_puts("Diving into user-mode\n");
	activate(first_stack_start);
	uart_puts("In kernel-mode\n");

	while (1);
	return 0;
}

