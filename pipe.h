#ifndef PIPE_H
#define PIPE_H

#include <scheduler.h>

#define PIPE_LIMIT (TASK_LIMIT*5)

void pipe_init(void);

void handle_read(unsigned *task);
void handle_write(unsigned *task);

#endif /* PIPE_H */
