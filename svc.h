#ifndef SVC_H
#define SVC_H

#define STACK_BASE 3
#define STATE (0)
#define CPSR (1)
#define PC (2)
#define r0 (STACK_BASE + 0)
#define r1 (STACK_BASE + 1)
#define r2 (STACK_BASE + 2)
#define r3 (STACK_BASE + 3)
#define r7 (STACK_BASE + 7)

typedef void svc_handler(unsigned *stack);

#endif /* SVC_H */
