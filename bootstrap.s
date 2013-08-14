.global _start
_start:
	mov r0, #0x00
	ldr r1, =interrupt_table
	ldr r3, =interrupt_table_end
keep_loading:
	ldr r2, [r1, #0x0]
	str r2, [r0, #0x0]
	add r0, r0, #0x4
	add r1, r1, #0x4
	cmp r1, r3
	bne keep_loading

	ldr sp, =0x07FFFFFF
	bl main

GIC0_INTERFACE: .word 0x1E000000
GIC0_DIST: .word 0x1E001000
TIMER0_IRQ_MASK: .word 0x10

.global enable_timer_int
enable_timer_int:
	/* enable timer interrupts */
	ldr r1, GIC0_DIST
	ldr r0, [r1, #0x104]
	ldr r3, TIMER0_IRQ_MASK
	orr r0, r0, r3
	str r0, [r1, #0x104]
	bx lr

.global init_int
init_int:
	/* enable cpu interface */
	ldr r1, GIC0_INTERFACE
	ldr r0, =0x1
	str r0, [r1, #0x000]
	/* write prioriry mask */
	ldr r0, =0xF0 /* handle all interrupt priorities */
	str r0, [r1, #0x004]
	/* enable interrupts */
	ldr r1, GIC0_DIST
	ldr r0, =0x1
	str r0, [r1, #0x0]

	bx lr

interrupt_table:
/* 0 */	nop
	nop
	ldr pc, svc_entry_address
	nop
/* 5 */ nop
	nop
	ldr pc, irq_entry_address

	svc_entry_address: .word svc_entry
	irq_entry_address: .word irq_entry
interrupt_table_end:

