#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stddef.h>

void sys_yield(void);
int sys_fork(void);
int sys_getpid(void);
int sys_write(int fd, const void *buf, size_t count);
int sys_read(int fd, void *buf, size_t count);
int sys_register_isr(unsigned line, int (*handler)(unsigned line));
void *sys_page_alloc(unsigned order);
void sys_page_free(void *addr, unsigned order);
int sys_exec(const char *path);
void sys_exit(int status);

#endif /* SYSCALLS_H */
