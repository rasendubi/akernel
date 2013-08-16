#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stddef.h>

#define TASK_LIMIT 16

#define TASK_READY      0
#define TASK_WAIT_READ  1
#define TASK_WAIT_WRITE 2

#define STACK_BASE 2
#define r0 (STACK_BASE + 0)
#define r1 (STACK_BASE + 1)
#define r2 (STACK_BASE + 2)
#define r3 (STACK_BASE + 3)
#define r7 (STACK_BASE + 7)

void add_task(void (*start)(void));
void schedule();

unsigned get_preempt_reason(void);

void handle_fork(void);
void handle_getpid(void);

extern unsigned int *tasks[];

extern size_t cur_task;
extern size_t task_count;

#endif /* SCHEDULER_H */

