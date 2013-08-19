#ifndef PRINT_H
#define PRINT_H

void printa(const char *format, ...)
	__attribute__((format(printf, 1, 2)));

#endif /* PRINT_H */

