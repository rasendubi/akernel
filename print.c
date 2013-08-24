#include <print.h>

#include <stdarg.h>

#include <uart.h>

static void print_hex_digit(int h, int upper) {
	const int basea = upper ? 'A' : 'a';
	uart_putchar((h < 10 ? '0' : basea) + h%10);
}

static void print_hex(unsigned number, int upper) {
	uart_puts("0x");
	for (int i = 28; i >= 0; i -= 4) {
		print_hex_digit( (number>>i)&0xf, upper);
	}
}

void dump_region(void *start, unsigned size) {
	unsigned line = 32;
	char *t = start;
	for (int i = 0; i < size; ++i) {
		if (i % line == 0) {
			printa("\n%x\t ", t + i);
		}
		print_hex_digit((t[i] >> 4) & 0xf, 0);
		print_hex_digit(t[i] & 0xf, 0);
		printa(" ");
	}
	printa("\n");
}

void printa(const char *format, ...) {
	va_list va;
	va_start(va, format);

	for (int i = 0; format[i]; ++i) {
		if (format[i] != '%') {
			uart_putchar(format[i]);
		} else {
			++i;
			if (format[i] == '%') {
				uart_putchar('%');
			} else if (format[i] == 's') {
				uart_puts(va_arg(va, const char *));
			} else if (format[i] == 'x') {
				print_hex(va_arg(va, unsigned), 0);
			} else if (format[i] == 'X') {
				print_hex(va_arg(va, unsigned), 1);
			} else if (format[i] == 'c') {
				uart_putchar(va_arg(va, unsigned));
			}
		}
	}

	va_end(va);
}
