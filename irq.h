#ifndef IRQ_H
#define IRQ_H

/*
 * Mask interrupt on given line.
 * Masking is countable (i.e. you should unmask interrupt as many times, as you
 * mask it. Unmaskable interrupts cannot be masked.
 * @param line interrupt line to be masked
 * @return 1 on success, 0 on fail.
 */
int mask_irq(unsigned line);
int unmask_irq(unsigned line);

void register_isr(unsigned line, int (*handler)(unsigned line));

#endif /* IRQ_H */
