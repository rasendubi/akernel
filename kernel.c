#include <uart.h>
#include <asm.h>

#define STACK_SIZE 256
#define TASK_LIMIT 2

unsigned int stacks[TASK_LIMIT][STACK_SIZE];
unsigned int *tasks[TASK_LIMIT];

unsigned int *init_task(unsigned int *stack, void (*start)(void)) {
	stack += STACK_SIZE - 16;
	stack[0] = 0x10; /* User mode, interrupts on */
	stack[1] = (unsigned int)start;
	return stack;
}

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
	tasks[0] = init_task(stacks[0], user_first);
	tasks[1] = init_task(stacks[1], task);

	activate(tasks[0]);
	activate(tasks[1]);

	while (1);
	return 0;
}

