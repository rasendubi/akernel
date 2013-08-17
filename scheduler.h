#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stddef.h>

#define TASK_LIMIT 16

#define TASK_READY      0
#define TASK_WAIT_READ  1
#define TASK_WAIT_WRITE 2

void add_task(void (*start)(void));
void schedule();

unsigned get_preempt_reason(void);

extern unsigned int *tasks[];

extern size_t cur_task;
extern size_t task_count;

#endif /* SCHEDULER_H */

