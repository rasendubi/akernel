#include <user.h>

#include <alloc.h>
#include <asm.h>
#include <print.h>
#include <uart.h>

static void irq_test(void);

static void task(void) {
	char message[200] = "Hello from task\n";
	printa("In other task\n");
	write(0, message, 100);
	while (1);
}

void alloc_test(void) {
	printa("Start allocation\n");
	char *b1 = malloc(0);
	char *b2 = malloc(4);
	char *b3 = malloc(100);
	char *b4 = malloc(100000);
	char *b5 = malloc(1000000);
	printa("Allocated\n");
	free(b4);
	free(b2);
	free(b3);
	free(b1);
	free(b5);
	printa("Freed\n");
	while (1);
}

void print_test(void) {
	printa("Plain: Hello, world!\n");
	printa("String: Hello %s, %s!\n", "one", "two");
	printa("Hex down: %x %x %x\n", 0x0, 0xdeadbeaf, 0xf000f000);
	printa("Hex up: %X %X %X\n", 0x0, 0xdeadbeaf, 0xf000f000);
	printa("Escaping: %% %s %%\n", "done");
	printa("Chars: %c%c%c%c\n", 'c', 'o', 'o', 'l');
	while (1);
}

void user_first(void) {
	char *buf = malloc(100);
	printa("In user mode\n");
	if (!fork()) irq_test();
	if (!fork()) task();
	if (!fork()) alloc_test();
	if (!fork()) print_test();
	printa("In user mode again\n");
	read(0, buf, 100);
	printa("Message read: %s", buf);
	while (1);
}

static int ticks;
static int user_isr(unsigned line) {
	(void)line;
	if (++ticks%8 == 0) {
		printa("%x ticks\n", ticks);
	}
	return 0;
}

static void irq_test(void) {
	ticks = 0;
	sys_register_isr(36, &user_isr);
	while (1);
}
