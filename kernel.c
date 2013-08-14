#include <stddef.h>

#include <uart.h>
#include <asm.h>

#define STACK_SIZE 256
#define TASK_LIMIT 16

unsigned int stacks[TASK_LIMIT][STACK_SIZE];
unsigned int *tasks[TASK_LIMIT];

unsigned int *init_task(unsigned int *stack, void (*start)(void)) {
	stack += STACK_SIZE - 16;
	stack[0] = 0x10; /* User mode, interrupts on */
	stack[1] = (unsigned int)start;
	return stack;
}

void *memcpy(void *dest, const void *src, size_t n) {
	char *d = dest;
	const char *s = src;
	size_t i;
	for (i = 0; i < n; ++i) {
		d[i] = s[i];
	}
	return d;
}

void task(void) {
	uart_puts("In other task\n");
	while (1) yield();
}

void user_first(void) {
	uart_puts("In user mode\n");
	if (!fork()) task();
	uart_puts("In user mode again\n");
	while (1) yield();
}

int main(void) {
	size_t task_count = 0;
	size_t cur_task = 0;

	tasks[0] = init_task(stacks[0], user_first);

	task_count = 1;

	while (1) {
		tasks[cur_task] = activate(tasks[cur_task]);

		/* Process sycalls */
		switch (tasks[cur_task][2+7]) {
		case 0x0: /* yield */
			break;
		case 0x1: /* fork */
			uart_puts("fork!\n");
			if (task_count == TASK_LIMIT) {
				tasks[cur_task][2+0] = -1;
			} else {
				size_t used = stacks[cur_task] + STACK_SIZE -
					tasks[cur_task];
				tasks[task_count] = stacks[task_count] +
					STACK_SIZE - used;
				memcpy(tasks[task_count], tasks[cur_task],
						used*sizeof(*tasks[cur_task]));
				tasks[cur_task][2+0] = task_count;
				tasks[task_count][2+0] = 0;
				++task_count;
			}
			break;
		default:
			uart_puts("Unknown syscall!!!");
		}

		++cur_task;
		if (cur_task >= task_count)
			cur_task = 0;
	}

	return 0;
}

