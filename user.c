#include <user.h>

#include <alloc.h>
#include <asm.h>
#include <print.h>
#include <uart.h>

static void task(void) {
	char message[200] = "Hello from task\n";
	uart_puts("In other task\n");
	write(0, message, 100);
	while (1);
}

void alloc_test(void) {
	uart_puts("Start allocation\n");
	char *b1 = malloc(0);
	char *b2 = malloc(4);
	char *b3 = malloc(100);
	char *b4 = malloc(100000);
	char *b5 = malloc(1000000);
	uart_puts("Allocated\n");
	free(b4);
	free(b2);
	free(b3);
	free(b1);
	free(b5);
	uart_puts("Freed\n");
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
	uart_puts("In user mode\n");
	if (!fork()) task();
	if (!fork()) alloc_test();
	if (!fork()) print_test();
	uart_puts("In user mode again\n");
	read(0, buf, 100);
	uart_puts("Message read: ");
	uart_puts(buf);
	while (1);
}

