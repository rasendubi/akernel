#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stddef.h>

#define TASK_READY      0
#define TASK_WAIT_READ  1

void init_scheduler(void);

void add_task(void (*entry_point)(void));
void schedule(void);

typedef struct task_struct {
	unsigned *kernel_stack;
	unsigned *kernel_stack_start;

	unsigned *stack;
	unsigned *stack_start;
	unsigned state;
	unsigned pid;

	struct task_struct *next;
	struct task_struct *prev;
} task_struct;

extern task_struct *cur_task;
extern task_struct *first_task;

#endif /* SCHEDULER_H */
