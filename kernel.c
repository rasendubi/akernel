#include <gic.h>
#include <page_alloc.h>
#include <pipe.h>
#include <scheduler.h>
#include <timer.h>
#include <uart.h>
#include <user.h>

int main(void) {
	init_int();
	enable_timer0_int();

	*TIMER0 = 1000000;
	*(TIMER0 + TIMER_CONTROL) = TIMER_EN | TIMER_PERIODIC | TIMER_32BIT |
		TIMER_INTEN;

	init_page_alloc();
	pipe_init();

	add_task(&user_first);

	while (1) {
		schedule();

		switch (get_preempt_reason()) {
		case 0x0: /* yield */
			break;
		case 0x1: /* fork */
			handle_fork();
			break;
		case 0x2: /* getpid */
			handle_getpid();
			break;
		case 0x3: /* write */
			handle_write(tasks[cur_task]);
			break;
		case 0x4: /* read */
			handle_read(tasks[cur_task]);
			break;

		case -36: /* Timer 0 or 1 went off */
			if (*(TIMER0 + TIMER_MIS)) {
				*(TIMER0 + TIMER_INTCLR) = 1;
			} else {
				uart_puts("Error: Unknown timer tick\n");
			}
			int_end(36);
			break;

		default:
			uart_puts("Error: Unknown preemption reason!\n");
		}
	}

	return 0;
}

