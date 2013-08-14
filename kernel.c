#include <uart.h>
#include <asm.h>

unsigned int stacks[2][256];
unsigned int *tasks[2];

void user_first(void) {
	uart_puts("In user mode\n");
	syscall();
	uart_puts("In user mode again\n");
	while (1) syscall();
}

void task(void) {
	uart_puts("In other task\n");
	while (1) syscall();
}

int main(void) {
	tasks[0] = stacks[0] + 256 - 16;
	tasks[0][0] = 0x10;
	tasks[0][1] = (unsigned int)&user_first;

	tasks[1] = stacks[1] + 256 - 16;
	tasks[1][0] = 0x10;
	tasks[1][1] = (unsigned int)&task;

	activate(tasks[0]);
	activate(tasks[1]);

	while (1);
	return 0;
}

