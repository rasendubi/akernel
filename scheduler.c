#include <scheduler.h>

#include <stddef.h>

#include <asm.h>
#include <page_alloc.h>
#include <svc.h>

#define STACK_SIZE (PAGE_SIZE/sizeof(unsigned))

static unsigned int *stacks[TASK_LIMIT];
unsigned int *tasks[TASK_LIMIT];

size_t cur_task = -1;
size_t task_count = 0;

static void *memcpy(void *dest, const void *src, size_t n) {
	char *d = dest;
	const char *s = src;
	size_t i;
	for (i = 0; i < n; ++i) {
		d[i] = s[i];
	}
	return d;
}

static unsigned int *init_task(unsigned int *stack, void (*start)(void)) {
	stack += STACK_SIZE - 16;
	stack[CPSR] = 0x10; /* User mode, interrupts on */
	stack[PC] = (unsigned int)start;
	return stack;
}

void add_task(void (*start)(void)) {
	tasks[task_count] = page_alloc(0);
	tasks[task_count] = init_task(stacks[task_count], start);
	++task_count;
}

void schedule(void) {
	do {
		cur_task = (cur_task + 1)%task_count;
	} while (tasks[cur_task][STATE] != TASK_READY);

	tasks[cur_task] = activate(tasks[cur_task]);
}

void handle_yield(unsigned *stack) {
	stack[STATE] = TASK_READY;
	return;
}

void handle_fork(unsigned *stack) {
	if (task_count == TASK_LIMIT) {
		stack[r0] = -1;
	} else {
		size_t used = stacks[cur_task] + STACK_SIZE - stack;
		stacks[task_count] = page_alloc(0);
		tasks[task_count] = stacks[task_count] + STACK_SIZE - used;
		memcpy(tasks[task_count], stack,
				used*sizeof(*stack));
		stack[r0] = task_count;
		tasks[task_count][r0] = 0;
		++task_count;
	}
}

void handle_getpid(unsigned *stack) {
	stack[r0] = cur_task;
}

