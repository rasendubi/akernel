#include "slab.h"
#include "slab_alloc.h"

#include <user/syscalls.h>

#include <print.h>

void _start(void) {
	int **arr = (int **)slab_alloc(sizeof(int*) * 50);
	for (int i = 0; i < 50; i++) {
		arr[i] = (int*)slab_alloc(sizeof(int));
		//printa("allocated %x\n", arr[i]);
		*arr[i] = i*i;
	}
	// slab_mem_dump();
	arr = (int**)slab_realloc(arr, sizeof(int*) * 100);
	slab_mem_dump();
	printa("Stupid printing\n");
	for (int i = 0; i < 50; i++) {
		printa("%x\n", *arr[i]);
		slab_free(arr[i]);
	}
	arr = (int**)slab_realloc(arr, sizeof(int*) * 10);
	size_t size = 5;
	int ar = (int)slab_alloc(size);
	// slab_mem_dump();
	slab_free((void*)ar);
	// slab_mem_dump();
	slab_free(arr);
	// slab_mem_dump();
	sys_exit(0);
}
