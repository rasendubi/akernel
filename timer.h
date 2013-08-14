#ifndef TIMER_H
#define TIMER_H

#define TIMER0 ((volatile unsigned int*)0x10011000)
#define TIMER_VALUE 0x1
#define TIMER_CONTROL 0x2
#define TIMER_INTCLR 0x3
#define TIMER_MIS 0x5

#define TIMER_EN       (1 << 7)
#define TIMER_PERIODIC (1 << 6)
#define TIMER_INTEN    (1 << 5)
#define TIMER_32BIT    (1 << 1)
#define TIMER_ONESHOT  (1 << 0)

#endif /* TIMER_H */
