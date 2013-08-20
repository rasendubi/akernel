#include <svc.h>

#include <stddef.h>

#include <scheduler.h>
#include <uart.h>

extern svc_handler *svc_handlers[];
extern svc_handler *end_svcs;

void handle_svc(unsigned svc, unsigned *stack) {
	tasks[cur_task].stack = stack;

	if (svc >= &end_svcs - svc_handlers) {
		uart_puts("Error: unknown SVC\n");
		stack[r0] = -1;
		return;
	} else if (svc_handlers[svc] == NULL) {
		uart_puts("Error: handler for SVC is not set\n");
		stack[r0] = -1;
		return;
	}

	svc_handlers[svc](&tasks[cur_task]);
}
