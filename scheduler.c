#include <scheduler.h>

#include <stddef.h>

#include <asm.h>
#include <gic.h>
#include <irq.h>
#include <page_alloc.h>
#include <svc.h>
#include <timer.h>
#include <utils.h>
#include <alloc.h>

#define USER_STACK_POWER 1
#define KERNEL_STACK_POWER 1

task_struct *cur_task = 0;
task_struct *first_task = 0;

static unsigned next_pid = 0;

static void first_run(task_struct *prev, task_struct *cur) {
	(void)prev;
	(void)cur;
	activate(cur_task->stack, cur_task->kernel_stack_begin);
}

extern void activate_kernel_restore(void);

static task_struct *task_new(void (*entry_point)(void)) {
	task_struct *new_task = (task_struct *)page_alloc(KERNEL_STACK_POWER);

	new_task->stack_start = page_alloc(USER_STACK_POWER);
	unsigned *stack = (unsigned *)((unsigned)new_task->stack_start + (PAGE_SIZE << USER_STACK_POWER) - 16*sizeof(unsigned));
	new_task->stack = stack;
	stack[CPSR] = 0x10;
	stack[PC] = (unsigned)entry_point;
	new_task->state = TASK_READY;

	new_task->pid = next_pid++;

	new_task->kernel_stack_begin = (unsigned *)((unsigned)new_task + (PAGE_SIZE << KERNEL_STACK_POWER));
	unsigned *kernel_stack = new_task->kernel_stack_begin - 16;
	new_task->kernel_stack = kernel_stack;
	kernel_stack[CPSR] = 0xD3;
	kernel_stack[PC] = (unsigned)&activate_kernel_restore;
	kernel_stack[lr] = (unsigned)first_run;

	return new_task;
}

void add_task(void (*entry_point)(void)) {
	task_struct *new_task = task_new(entry_point);

	if (first_task == 0) {
		first_task = new_task;
		new_task->prev = new_task;
		new_task->next = new_task;
	} else {
		first_task->prev->next = new_task;
		first_task->prev = new_task;
	}
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

static task_struct *peek_next_task(void) {
	task_struct *result = cur_task;
	do {
		result = result ? result->next : first_task;
	} while (result->state != TASK_READY);
	return result;
}

void schedule(void) {
	flags &= ~NEED_RESCHED;
	task_struct *next = peek_next_task();
	if (cur_task != next) {
		task_struct *prev = cur_task;
		cur_task = next;

		activate_kernel(prev, next);
	}
	activate(cur_task->stack, cur_task->kernel_stack_begin);
}

void handle_yield(task_struct *ts) {
	(void)ts;
	flags |= NEED_RESCHED;
}

void handle_fork(task_struct *ts) {
	unsigned *stack = ts->stack;

	size_t used = cur_task->stack_start + (PAGE_SIZE << USER_STACK_POWER)/sizeof(unsigned) - stack;
	task_struct *new_task = task_new(NULL);

	new_task->stack = new_task->stack_start + (PAGE_SIZE << USER_STACK_POWER)/sizeof(unsigned) - used;
	memcpy(new_task->stack, stack, used*sizeof(*stack));

	stack[r0] = new_task->pid;
	new_task->stack[r0] = 0;

	new_task->prev = first_task->prev;
	new_task->next = first_task;
	first_task->prev->next = new_task;
	first_task->prev = new_task;
}

void handle_getpid(task_struct *ts) {
	unsigned *stack = ts->stack;
	stack[r0] = cur_task->pid;
}

void handle_exit(task_struct *ts) {
	ts->prev->next = ts->next;
	ts->next->prev = ts->prev;
	if (ts->next == ts) {
		first_task = 0; // There are no other tasks
	}
	flags |= NEED_RESCHED;

	page_free(ts->stack_start, USER_STACK_POWER);
	page_free(ts, KERNEL_STACK_POWER);
}

void irq_return(unsigned *stack) {
	cur_task->stack = stack;
	if (flags & NEED_RESCHED) {
		schedule();
	}
	activate(cur_task->stack, cur_task->kernel_stack_begin);
}
