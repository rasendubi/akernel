#include <pipe_master.h>

#include <alloc.h>
#include <asm.h>
#include <pipe.h>
#include <utils.h>

static char *paths[PIPE_LIMIT] = { "/sys/pipe_master" };
static int n_paths = 1;

static int open_pipe(const char *name) {
	for (int i = 0; i < n_paths; ++i) {
		if (strcmp(name, paths[i]) == 0) {
			return i + TASK_LIMIT;
		}
	}
	return -1;

}

void pipe_master(void) {
	while (1) {
		unsigned reply_pipe;
		unsigned plen;
		unsigned op;
		read(MASTER_PIPE, &reply_pipe, sizeof(reply_pipe));
		read(MASTER_PIPE, &op, sizeof(op));
		read(MASTER_PIPE, &plen, sizeof(plen));
		char *name = malloc(plen);
		read(MASTER_PIPE, name, plen);
		if (op == 0) {
			paths[n_paths++] = name;
			int i = n_paths + TASK_LIMIT - 1;
			write(reply_pipe, &i, sizeof(i));
		} else {
			int i = open_pipe(name);
			write(reply_pipe, &i, sizeof(i));
			free(name);
		}
	}
}
