#ifndef ASM_H
#define ASM_H

unsigned int *activate(unsigned int *stack);

void init_int(void);
void enable_timer_int(void);

void yield(void);
int fork(void);
int getpid(void);

#endif /* ASM_H */
