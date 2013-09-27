#include <alloc.h>
#include <print.h>

int _start(void) {
	printa("Start allocation\n");
	mem_dump();
	char *b1 = malloc(0);
	char *b2 = malloc(4);
	char *b3 = malloc(100);
	char *b4 = malloc(100000);
	char *b5 = malloc(1000000);
	printa("Allocated\n");
	mem_dump();
	b1 = realloc(b1, 4);
	b2 = realloc(b2, 0);
	b3 = realloc(b3, 1000);
	b4 = realloc(b4, 100000);
	b5 = realloc(b5, 500000);
	printa("Reallocated\n");
	mem_dump();
	char *b6 = calloc(100, 4);
	printa("Calloced\n");
	mem_dump();
	free(b4);
	free(b2);
	free(b3);
	free(b1);
	free(b5);
	free(b6);
	printa("Freed\n");
	mem_dump();
	while (1);
}
