#include <pipe.h>

#include <svc.h>

#define PIPE_BUF   512

struct pipe_ringbuffer {
	int start;
	int end;
	char data[PIPE_BUF];
};

#define RB_PUSH(rb, size, v) do {            \
		(rb).data[(rb).end] = (v);   \
		(rb).end++;                  \
		if ((rb).end > size)         \
			(rb).end = 0;        \
	} while (0)

#define RB_POP(rb, size, v) do {             \
		(v) = (rb).data[(rb).start]; \
		(rb).start++;                \
		if ((rb).start > size)       \
			(rb).start = 0;      \
	} while (0)

#define RB_LEN(rb, size) (((rb).end - (rb).start) + \
		((rb).end < (rb).start ? size : 0))

#define PIPE_PUSH(pipe, v) RB_PUSH((pipe), PIPE_BUF, (v))
#define PIPE_POP(pipe, v)  RB_POP((pipe), PIPE_BUF, (v))
#define PIPE_LEN(pipe)      RB_LEN((pipe), PIPE_BUF)

struct pipe_ringbuffer pipes[PIPE_LIMIT];

void handle_read(task_struct *ts);
void handle_write(task_struct *ts);

void init_pipes(void) {
	size_t i;
	for (i = 0; i < PIPE_LIMIT; ++i) {
		pipes[i].start = pipes[i].end = 0;
	}
}

void handle_read(task_struct *ts) {
	struct pipe_ringbuffer *pipe;
	unsigned *task = ts->stack;

	ts->state = TASK_READY;
	if (task[r0] > PIPE_LIMIT || task[r2] > PIPE_BUF) {
		task[r0] = -1;
		return;
	}

	pipe = &pipes[task[r0]];
	if ((size_t)PIPE_LEN(*pipe) < task[r2]) {
		ts->state = TASK_WAIT_READ;
	} else {
		size_t i;
		char *buf = (char*)task[r1];
		for (i = 0; i < task[r2]; ++i) {
			PIPE_POP(*pipe, buf[i]);
		}

		for (i = 0; i < task_count; ++i) {
			if (tasks[i].state == TASK_WAIT_WRITE) {
				handle_write(&tasks[i]);
			}
		}
	}
}

void handle_write(task_struct *ts) {
	struct pipe_ringbuffer *pipe;
	unsigned *task = ts->stack;

	if (task[r0] > PIPE_LIMIT || task[r2] > PIPE_BUF) {
		task[r0] = -1;
		return;
	}

	pipe = &pipes[task[r0]];
	if ((size_t)PIPE_BUF - PIPE_LEN(*pipe) < task[r2]) {
		ts->state = TASK_WAIT_WRITE;
	} else {
		size_t i;
		const char *buf = (const char *)task[r1];
		for (i = 0; i < task[r2]; ++i) {
			PIPE_PUSH(*pipe, buf[i]);
		}

		for (i = 0; i < task_count; ++i) {
			if (tasks[i].state == TASK_WAIT_READ) {
				handle_read(&tasks[i]);
			}
		}
	}
}
