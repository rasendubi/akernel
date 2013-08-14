#include <uart.h>

#define UART0 ((volatile char *)0x10009000)
#define UARTFR 0x06
#define UARTFR_TXFF 0x20

void uart_puts(char *s) {
	while (*s) {
		while (*(UART0 + UARTFR) & UARTFR_TXFF);
		*UART0 = *s;
		++s;
	}
}

