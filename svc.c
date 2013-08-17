#include <svc.h>

#include <stddef.h>

#include <scheduler.h>
#include <uart.h>

#define SVC_COUNT 5

static svc_handler *svc_handlers[SVC_COUNT];

void register_svc(unsigned n, svc_handler *handler) {
	if (svc_handlers[n] != NULL) {
		uart_puts("Error: reregister SVC handler\n");
		return;
	}
	svc_handlers[n] = handler;
}

void handle_svc(unsigned *stack) {
	unsigned int svc = stack[r7];

	tasks[cur_task] = stack;

	if (svc >= SVC_COUNT) {
		uart_puts("Error: unknown SVC\n");
		stack[r0] = -1;
		return;
	} else if (svc_handlers[svc] == NULL) {
		uart_puts("Error: handler for SVC is not set\n");
		stack[r0] = -1;
		return;
	}

	svc_handlers[svc](stack);
}

