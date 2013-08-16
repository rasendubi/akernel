#include <gic.h>

#define GIC0_INTERFACE_BASE 0x1E000000
#define GIC0_CPU_CONTROL ((volatile unsigned *)(GIC0_INTERFACE_BASE + 0x000))
#define GIC0_PRIORITY_MASK ((volatile unsigned *)(GIC0_INTERFACE_BASE + 0x004))
#define GIC0_INT_END ((volatile unsigned int*)(GIC0_INTERFACE_BASE + 0x010))

#define GIC0_DIST_BASE 0x1E001000
#define GIC0_DIST_CONTROL ((volatile unsigned int*)(GIC0_DIST_BASE + 0x000))
#define GIC0_SET_ENABLE1 ((volatile unsigned int*)(GIC0_DIST_BASE + 0x104))

#define TIMER0_IRQ (1 << 4)

void init_int(void) {
	*(GIC0_CPU_CONTROL) = 1;
	*(GIC0_PRIORITY_MASK) = 0xF << 4;;

	*GIC0_DIST_CONTROL = 1;
}

void enable_timer0_int(void) {
	*(GIC0_SET_ENABLE1) |= TIMER0_IRQ;
}

void int_end(unsigned n) {
	*GIC0_INT_END = n;
}


