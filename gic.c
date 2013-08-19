#include <gic.h>

#define GIC0_INTERFACE_BASE 0x1E000000
#define GIC0_CPU_CONTROL ((volatile unsigned *)(GIC0_INTERFACE_BASE + 0x000))
#define GIC0_PRIORITY_MASK ((volatile unsigned *)(GIC0_INTERFACE_BASE + 0x004))
#define GIC0_INT_END ((volatile unsigned int*)(GIC0_INTERFACE_BASE + 0x010))

#define GIC0_DIST_BASE 0x1E001000
#define GIC0_DIST_CONTROL ((volatile unsigned int*)(GIC0_DIST_BASE + 0x000))
#define GIC0_SET_ENABLE0 ((volatile unsigned *)(GIC0_DIST_BASE + 0x100))
#define GIC0_CLEAR_ENABLE0 ((volatile unsigned *)(GIC0_DIST_BASE + 0x184))

void init_int(void) {
	*GIC0_CPU_CONTROL = 1;
	*GIC0_PRIORITY_MASK = 0xF << 4;;

	*GIC0_DIST_CONTROL = 1;
}

void enable_int(unsigned n) {
	volatile unsigned *set_enable = GIC0_SET_ENABLE0 + n/32;
	*set_enable |= 1 << (n%32);
}

void disable_int(unsigned n) {
	volatile unsigned *clear_enable = GIC0_CLEAR_ENABLE0 + n/32;
	*clear_enable |= 1 << (n%32);
}

void int_end(unsigned n) {
	*GIC0_INT_END = n;
}
