#ifndef SCHEDULER_H
#define SCHEDULER_H

void add_task(void (*start)(void));
void schedule();

unsigned get_preempt_reason(void);

void handle_fork(void);
void handle_getpid(void);

#endif /* SCHEDULER_H */

