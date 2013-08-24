volatile int glob = 10,
	 a = 5, b = 4;

int main(void) {
	glob = a + b;
	uart_puts("Hello from stupid!!!!\n");
	while (1);
}

int first(void) {
	return glob = a + b;
}
