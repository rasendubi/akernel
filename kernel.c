#include <uart.h>

int main(void) {
	uart_puts("Hello, world!\n");

	while (1);
	return 0;
}

