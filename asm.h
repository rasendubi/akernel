#ifndef ASM_H
#define ASM_H

#include <stddef.h>

unsigned int *activate(unsigned int *stack);

void yield(void);
int fork(void);
int getpid(void);
int write(int fd, const void *buf, size_t count);
int read(int fd, void *buf, size_t count);

#endif /* ASM_H */
