#include <scheduler.h>

#include <stddef.h>

#include <asm.h>
#include <gic.h>
#include <irq.h>
#include <page_alloc.h>
#include <svc.h>
#include <timer.h>

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

#define NEED_RESCHED 1
unsigned flags;

static int timer_handler(unsigned unused) {
	(void)unused;
	if (!*(TIMER0 + TIMER_MIS)) {
		return 0;
	}

	*(TIMER0 + TIMER_INTCLR) = 1;
	flags |= NEED_RESCHED;
	return 1;
}

void init_scheduler(void) {
	register_isr(TIMER0_INT, timer_handler);
	enable_int(TIMER0_INT);

	*TIMER0 = 100000;
	*(TIMER0 + TIMER_CONTROL) = TIMER_EN | TIMER_PERIODIC | TIMER_32BIT |
		TIMER_INTEN;

}

void schedule(void) {
	do {
		cur_task = (cur_task + 1)%task_count;
	} while (tasks[cur_task][STATE] != TASK_READY);

	tasks[cur_task] = activate(tasks[cur_task]);
	flags &= ~NEED_RESCHED;
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

