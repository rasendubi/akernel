#include <scheduler.h>

#include <stddef.h>

#include <asm.h>

#define STACK_SIZE 256
#define TASK_LIMIT 16

#define TASK_READY      0
#define TASK_WAIT_READ  1
#define TASK_WAIT_WRITE 2

static unsigned int stacks[TASK_LIMIT][STACK_SIZE];
static unsigned int *tasks[TASK_LIMIT];

static size_t cur_task = -1;
static size_t task_count = 0;

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
	stack[0] = 0x10; /* User mode, interrupts on */
	stack[1] = (unsigned int)start;
	return stack;
}

void add_task(void (*start)(void)) {
	tasks[task_count] = init_task(stacks[task_count], start);
	++task_count;
}

void schedule(void) {
	do {
		++cur_task;
		if (cur_task == task_count) {
			cur_task = 0;
		}
	} while (tasks[cur_task][-1] != TASK_READY);

	tasks[cur_task] = activate(tasks[cur_task]);
	tasks[cur_task][-1] = TASK_READY;
}

unsigned get_preempt_reason(void) {
	return tasks[cur_task][2+7];
}

void handle_fork(void) {
	if (task_count == TASK_LIMIT) {
		tasks[cur_task][2+0] = -1;
	} else {
		size_t used = stacks[cur_task] + STACK_SIZE - tasks[cur_task];
		tasks[task_count] = stacks[task_count] + STACK_SIZE - used;
		memcpy(tasks[task_count], tasks[cur_task],
				used*sizeof(*tasks[cur_task]));
		tasks[cur_task][2+0] = task_count;
		tasks[task_count][2+0] = 0;
		++task_count;
	}
}

void handle_getpid(void) {
	tasks[cur_task][2+0] = cur_task;
}

