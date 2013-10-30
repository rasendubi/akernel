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

#define STACK_SIZE (PAGE_SIZE/sizeof(unsigned))

task_struct *cur_task = 0;
task_struct *first_task = 0;

unsigned next_pid = 0;

static task_struct init_task(unsigned int *stack, void (*entry_point)(void)) {
	task_struct ts;
	stack += STACK_SIZE - 15;
	stack[CPSR] = 0x10; /* User mode, interrupts on */
	stack[PC] = (unsigned int)entry_point;
	ts.stack = stack;
	ts.state = TASK_READY;
	return ts;
}

void add_task(void (*entry_point)(void)) {
	task_struct *new_task = malloc(sizeof(task_struct));
	unsigned *stack_start = page_alloc(0);
	*new_task = init_task(stack_start, entry_point);
	new_task->stack_start = stack_start;
	new_task->pid = next_pid++;

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

void schedule(void) {
	flags &= ~NEED_RESCHED;
	do {
		cur_task = cur_task ? cur_task->next : first_task;
	} while (cur_task->state != TASK_READY);
	cur_task->stack = activate(cur_task->stack);
}

void handle_yield(task_struct *ts) {
	(void)ts;
	flags |= NEED_RESCHED;
	return;
}

void handle_fork(task_struct *ts) {
	unsigned *stack = ts->stack;

	size_t used = cur_task->stack_start + STACK_SIZE - stack;
	task_struct *new_task = malloc(sizeof(task_struct));
	new_task->stack_start = page_alloc(0);
	new_task->stack = new_task->stack_start + STACK_SIZE - used;
	memcpy(new_task->stack, stack, used*sizeof(*stack));
	new_task->pid = next_pid++;
	stack[r0] = new_task->pid;
	new_task->stack[r0] = 0;
	new_task->state = ts->state;

	new_task->prev = first_task->prev;
	new_task->next = first_task;
	first_task->prev->next = new_task;
	first_task->prev = new_task;
}

void handle_getpid(task_struct *ts) {
	unsigned *stack = ts->stack;
	stack[r0] = cur_task->pid;
}
