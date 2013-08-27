#include <exec_elf.h>
#include <svc.h>
#include <tarfs.h>

extern char _binary_ramdisk_tar_start;

void handle_exec(task_struct *ts) {
	const char *path = (const char *)ts->stack[r0];

	void *file = tar_get_file(&_binary_ramdisk_tar_start, path);
	if (!file) {
		ts->stack[r0] = -1;
		return;
	}

	elf_object *obj = load_elf(file);
	ts->stack[PC] = (unsigned)obj->entry_point;
}
