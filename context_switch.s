GIC0: .word 0x1E000000
TASK_READY: .word 0

.type activate, %function
.global activate
activate:
	mov ip, sp
	push {r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}

	add r0, #4 /* skip STATE */
	ldmfd r0!, {ip,lr}
	msr SPSR, ip

	msr CPSR_c, #0xDF /* System mode */
	mov sp, r0
	pop {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
	msr CPSR_c, #0xD3 /* Supervisor mode */

	movs pc, lr

.global svc_entry
svc_entry:
	msr CPSR_c, #0xDF /* System mode */
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
	mov r0, r7
	mov r1, sp
	msr CPSR_c, #0xD3 /* Supervisor mode */

	mrs ip, SPSR
	ldr r4, TASK_READY
	stmfd r1!, {r4,ip,lr}

	push {r1}
	bl handle_svc
	pop {r0}

	pop {r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
	mov sp, ip
	bx lr

.global irq_entry
irq_entry:
	msr CPSR_c, #0xDF /* System mode */
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
	mov r0, sp

	msr CPSR_c, #0xD2 /* IRQ mode */
	mrs ip, SPSR
	sub lr, lr, #0x4
	ldr r4, TASK_READY
	stmfd r0!, {r4,ip,lr}

	msr CPSR_c, #0xD3 /* Supervisor mode */
	push {r0}

	ldr r0, GIC0
	ldr r0, [r0, #0x00C]

	bl handle_irq

	pop {r0}

	pop {r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
	mov sp, ip
	bx lr

