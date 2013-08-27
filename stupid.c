#include <uart.h>

volatile int glob = 10,
	 a = 5, b = 4;

int main(void) {
	glob = a + b;
	uart_puts("Hello from stupid!!!!\n");
	return 0;
}

int first(void) {
	return glob = a + b;
}
