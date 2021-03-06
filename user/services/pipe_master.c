#include <pipe_master.h>

#include <alloc.h>
#include <pipe.h>
#include <uart.h>
#include <utils.h>
#include <user/syscalls.h>

#define CUSTOM_PIPE_BASE 256

static char *paths[PIPE_LIMIT] = { "/sys/pipe_master" };
static int n_paths = 1;

static int open_pipe(const char *name) {
	for (int i = 0; i < n_paths; ++i) {
		if (strcmp(name, paths[i]) == 0) {
			return i ? i + CUSTOM_PIPE_BASE : 0;
		}
	}
	return -1;
}

int _start(void) {
	uart_puts("Pipe master started\n");
	while (1) {
		unsigned reply_pipe;
		unsigned plen;
		unsigned op;
		sys_read(MASTER_PIPE, &reply_pipe, sizeof(reply_pipe));
		sys_read(MASTER_PIPE, &op, sizeof(op));
		sys_read(MASTER_PIPE, &plen, sizeof(plen));
		char *name = malloc(plen);
		sys_read(MASTER_PIPE, name, plen);
		if (op == 0) {
			paths[n_paths] = name;
			int i = CUSTOM_PIPE_BASE + n_paths++;
			sys_write(reply_pipe, &i, sizeof(i));
		} else {
			int i = open_pipe(name);
			sys_write(reply_pipe, &i, sizeof(i));
			free(name);
		}
	}
	return -1;
}
