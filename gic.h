#ifndef GIC_H
#define GIC_H

#define TIMER0_INT 36

void init_int(void);

void enable_int(unsigned n);
void disable_int(unsigned n);

void enable_timer0_int(void);

void int_end(unsigned n);

#endif /* GIC_H */
