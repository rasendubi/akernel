.global activate
activate:
	mov ip, sp
	push {r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}

	ldmfd r0!, {ip,lr}
	msr SPSR, ip

	msr CPSR_c, #0xDF /* System mode */
	mov sp, r0
	pop {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
	pop {r7}
	msr CPSR_c, #0xD3 /* Supervisor mode */

	movs pc, lr

.global svc_entry
svc_entry:
	msr CPSR_c, #0xDF /* System mode */
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
	mov r0, sp
	msr CPSR_c, #0xD3

	mrs ip, SPSR
	stmfd r0!, {ip,lr}

	pop {r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
	mov sp, ip
	bx lr

GIC0: .word 0x1E000000

.global irq_entry
irq_entry:
	msr CPSR_c, #0xDF /* System mode */

	push {r7}
	push {r8}
	ldr r8, GIC0
	ldr r7, [r8, #0x00C]

	ldr r8, =0x0
	sub r7, r8, r7
	pop {r8}

	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
	mov r0, sp

	msr CPSR_c, #0xD2 /* IRQ mode */
	mrs ip, SPSR
	sub lr, lr, #0x4
	stmfd r0!, {ip,lr}

	msr CPSR_c, #0xD3

	pop {r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
	mov sp, ip
	bx lr

