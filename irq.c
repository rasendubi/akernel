#include <irq.h>

#include <alloc.h>
#include <gic.h>
#include <print.h>

#define NR_IRQS 128

unsigned irq_stack[2048];

typedef struct isr_struct {
	int (*handler)(unsigned line);
	struct isr_struct *next;
} isr_struct;

typedef struct irq_struct {
	int mask_count;
	isr_struct *isrs;
} irq_struct;

static irq_struct irqs[NR_IRQS];

unsigned irq_level;

void register_isr(unsigned line, int (*handler)(unsigned line)) {
	isr_struct *isr = malloc(sizeof(isr_struct));
	if (!isr) {
		printa("Error: can't allocate memory for isr\n");
		return;
	}

	isr->handler = handler;
	isr->next = irqs[line].isrs;
	irqs[line].isrs = isr;
}

void handle_irq(unsigned line) {
	if (irqs[line].isrs == 0) {
		printa("Error: no handler for interrupt %x\n", line);
	}

	int handled = 0;
	isr_struct *cur = irqs[line].isrs;
	while (cur) {
		handled |= cur->handler(line);
		cur = cur->next;
	}
	if (!handled) {
		printa("Interrupt on line %x not handled\n", line);
	}
	int_end(line);
}
