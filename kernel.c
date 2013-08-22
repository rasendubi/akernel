#include <asm.h>
#include <gic.h>
#include <page_alloc.h>
#include <pipe.h>
#include <pipe_master.h>
#include <scheduler.h>
#include <user.h>

unsigned hyp_stack[2048];

void init_systems(void) {
	if (!fork())
		user_first();

	pipe_master();
}

extern unsigned _binary_ramdisk_tar_start;

int main(void) {
	printa("In main %x\n", main);
	init_int();
	init_page_alloc();
	init_pipes();
	init_scheduler();

	unsigned *a = &_binary_ramdisk_tar_start + 0x200/4;
	printa("%x: %s\n", &_binary_ramdisk_tar_start,
			&_binary_ramdisk_tar_start);
	printa("%x: %s\n", a, a);

	add_task(&init_systems);

	while (1) {
		schedule();
	}

	return 0;
}
