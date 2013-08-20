#include <svc.h>

#include <stddef.h>

#include <scheduler.h>
#include <uart.h>

#define SVC_COUNT 5

extern svc_handler *svc_handlers[];

void handle_svc(unsigned svc, unsigned *stack) {
	tasks[cur_task].stack = stack;

	if (svc >= SVC_COUNT) {
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
