#include <asm.h>
#include <exec_elf.h>
#include <gic.h>
#include <page_alloc.h>
#include <pipe.h>
#include <pipe_master.h>
#include <print.h>
#include <scheduler.h>
#include <tarfs.h>
#include <user.h>

unsigned hyp_stack[2048];

void init_systems(void) {
	if (!fork())
		user_first();

	pipe_master();
}

extern unsigned _binary_ramdisk_tar_start;

int main(void) {
	printa("In main %x\n", (unsigned)main);
	init_int();
	init_page_alloc();
	init_pipes();
	init_scheduler();

	void *a = tar_get_file(&_binary_ramdisk_tar_start, "stupid");
	elf_object *obj = load_elf(a);
	printa("supid: %p\n", a);

	void *b = tar_get_file(&_binary_ramdisk_tar_start, "init.txt");
	printa("init.txt: %p\n", b);

	add_task(&init_systems);
	add_task((void (*)(void))obj->entry_point);

	while (1) {
		schedule();
	}

	return 0;
}
