#ifndef GIC_H
#define GIC_H

void init_int(void);
void int_enable(void);

void enable_timer0_int(void);

void int_end(unsigned n);

#endif /* GIC_H */
