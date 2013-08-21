#include <user_pipe_master.h>

#include <alloc.h>
#include <asm.h>
#include <utils.h>

int pipe_new(const char *name) {
	int name_len = strlen(name);
	int buf_len = name_len + 1 + 3*sizeof(unsigned);
	int pid = getpid();
	char *buf = malloc(buf_len);
	((unsigned *)buf)[0] = pid;
	((unsigned *)buf)[1] = 0;
	((unsigned *)buf)[2] = name_len + 1;
	memcpy(buf + 3*sizeof(unsigned), name, name_len + 1);
	write(0, buf, buf_len);
	int reply;
	read(pid, &reply, sizeof(reply));
	return reply;
}

int pipe_open(const char *name) {
	int name_len = strlen(name);
	int buf_len = name_len + 1 + 3*sizeof(unsigned);
	int pid = getpid();
	char *buf = malloc(buf_len);
	((unsigned *)buf)[0] = pid;
	((unsigned *)buf)[1] = 1;
	((unsigned *)buf)[2] = name_len + 1;
	memcpy(buf + 3*sizeof(unsigned), name, name_len + 1);
	write(0, buf, buf_len);
	int reply;
	read(pid, &reply, sizeof(reply));
	return reply;
}
