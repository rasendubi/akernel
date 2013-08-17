#include <gic.h>
#include <page_alloc.h>
#include <pipe.h>
#include <scheduler.h>
#include <timer.h>
#include <uart.h>
#include <user.h>

int main(void) {
	init_int();
	init_page_alloc();
	init_scheduler();
	init_pipes();
	enable_timer0_int();

	*TIMER0 = 1000000;
	*(TIMER0 + TIMER_CONTROL) = TIMER_EN | TIMER_PERIODIC | TIMER_32BIT |
		TIMER_INTEN;


	add_task(&user_first);

	while (1) {
		int reason;
		schedule();

		reason = (int)get_preempt_reason();
		if (reason == -36) {
			/* Timer 0 or 1 went off */
			if (*(TIMER0 + TIMER_MIS)) {
				*(TIMER0 + TIMER_INTCLR) = 1;
			} else {
				uart_puts("Error: Unknown timer tick\n");
			}
			int_end(36);
		} else if (reason < 0) {
			uart_puts("Error: Unknown preemption reason!\n");
		}
	}

	return 0;
}

