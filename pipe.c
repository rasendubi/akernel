#include <pipe.h>

#include <svc.h>
#include <growbuf.h>

growbuf *pipes[PIPE_LIMIT];

void handle_read(task_struct *ts);
void handle_write(task_struct *ts);

void init_pipes(void) {
	for (size_t i = 0; i < PIPE_LIMIT; ++i) {
		pipes[i] = growbuf_new();
	}
}

void handle_read(task_struct *ts) {
	unsigned *task = ts->stack;

	ts->state = TASK_READY;
	if (task[r0] > PIPE_LIMIT) {
		task[r0] = -1;
		return;
	}

	growbuf *pipe = pipes[task[r0]];
	if (grow_len(pipe) < task[r2]) {
		ts->state = TASK_WAIT_READ;
	} else {
		char *buf = (char*)task[r1];
		for (size_t i = 0; i < task[r2]; ++i) {
			buf[i] = grow_pop(pipe);
		}
	}
}

void handle_write(task_struct *ts) {
	unsigned *task = ts->stack;

	if (task[r0] > PIPE_LIMIT) {
		task[r0] = -1;
		return;
	}

	growbuf *pipe = pipes[task[r0]];
	const char *buf = (const char *)task[r1];
	for (size_t i = 0; i < task[r2]; ++i) {
		grow_push(pipe, buf[i]);
	}

	for (size_t i = 0; i < task_count; ++i) {
		if (tasks[i].state == TASK_WAIT_READ) {
			handle_read(&tasks[i]);
		}
	}
}
