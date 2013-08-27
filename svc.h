#ifndef SVC_H
#define SVC_H

#include <scheduler.h>

#define STACK_BASE 2
#define CPSR (0)
#define PC (1)
#define r0 (STACK_BASE + 0)
#define r1 (STACK_BASE + 1)
#define r2 (STACK_BASE + 2)
#define r3 (STACK_BASE + 3)
#define r7 (STACK_BASE + 7)
#define lr (STACK_BASE + 13)

typedef void svc_handler(task_struct *ts);

#endif /* SVC_H */
