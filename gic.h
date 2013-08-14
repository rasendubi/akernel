#ifndef GIC_H
#define GIC_H

#define GIC0_DIST ((volatile unsigned int*)0x1E001000)
#define GIC0_SET_ENABLE1 ((volatile unsigned int*)0x1E001104)
#define SET_ENABLE1_OFFSET 0x104
#define TIMER0_IRQ (1 << 4)

#endif /* GIC_H */
