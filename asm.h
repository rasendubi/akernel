#ifndef ASM_H
#define ASM_H

task_struct *activate_kernel(task_struct *cur, task_struct *next);
unsigned int *activate(unsigned int *stack, unsigned int *kernel_stack);

#endif /* ASM_H */
