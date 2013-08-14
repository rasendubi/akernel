.global activate
activate:
	msr CPSR_c, #0xDF /* System mode */
	mov sp, r0
	msr CPSR_c, #0xD3 /* Supervisor mode */

	mov r0, #0x10
	msr SPSR, r0
	ldr lr, =user_first

	movs pc, lr
