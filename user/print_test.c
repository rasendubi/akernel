#include <print.h>
void _start(void) {
	printa("Plain: Hello, world!\n");
	printa("String: Hello %s, %s!\n", "one", "two");
	printa("Hex down: %x %x %x\n", 0x0, 0xdeadbeaf, 0xf000f000);
	printa("Hex up: %X %X %X\n", 0x0, 0xdeadbeaf, 0xf000f000);
	printa("Escaping: %% %s %%\n", "done");
	printa("Chars: %c%c%c%c\n", 'c', 'o', 'o', 'l');
	while (1);
}