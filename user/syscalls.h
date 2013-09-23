#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stddef.h>

void yield(void);
int fork(void);
int getpid(void);
int write(int fd, const void *buf, size_t count);
int read(int fd, void *buf, size_t count);
int sys_register_isr(unsigned line, int (*handler)(unsigned line));
void *sys_page_alloc(unsigned order);
void sys_page_free(void *addr, unsigned order);
int sys_exec(const char *path);

#endif /* SYSCALLS_H */
