#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stddef.h>

#define TASK_LIMIT 16

#define TASK_READY      0
#define TASK_WAIT_READ  1

void init_scheduler(void);

void add_task(void (*start)(void));
void schedule();

struct task_struct;

typedef struct task_struct {
	unsigned *stack, *stack_beginning;
	unsigned state, pid;
	struct task_struct *next;
	struct task_struct *prev;
} task_struct;

extern task_struct tasks[];

extern task_struct *cur_task;
extern task_struct *first_task;

#endif /* SCHEDULER_H */
