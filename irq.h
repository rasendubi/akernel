#ifndef IRQ_H
#define IRQ_H

void register_isr(unsigned line, int (*handler)(unsigned line));

#endif /* IRQ_H */
