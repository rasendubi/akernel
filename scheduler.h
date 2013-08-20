#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stddef.h>

#define TASK_LIMIT 16

#define TASK_READY      0
#define TASK_WAIT_READ  1
#define TASK_WAIT_WRITE 2

void init_scheduler(void);

void add_task(void (*start)(void));
void schedule();

typedef struct task_struct {
	unsigned *stack;
	unsigned state;
} task_struct;

extern task_struct tasks[];

extern size_t cur_task;
extern size_t task_count;

#endif /* SCHEDULER_H */
