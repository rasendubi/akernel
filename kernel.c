#include <gic.h>
#include <irq.h>
#include <page_alloc.h>
#include <pipe.h>
#include <scheduler.h>
#include <timer.h>
#include <user.h>

static int timer_handler(unsigned unused) {
	if (!*(TIMER0 + TIMER_MIS)) {
		return 0;
	}

	*(TIMER0 + TIMER_INTCLR) = 1;
	return 1;
}

int main(void) {
	init_int();
	init_page_alloc();
	init_pipes();

	register_isr(36, timer_handler);
	enable_timer0_int();

	*TIMER0 = 100000;
	*(TIMER0 + TIMER_CONTROL) = TIMER_EN | TIMER_PERIODIC | TIMER_32BIT |
		TIMER_INTEN;


	add_task(&user_first);

	while (1) {
		schedule();
	}

	return 0;
}

