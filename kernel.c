#define UART0 ((volatile char *)0x10009000)

int main(void) {
	char *string = "Hello, world!\n";
	while (*string) {
		*UART0 = *string;
		string++;
	}

	while (1);
	return 0;
}
