GIC0: .word 0x1E000000
TASK_READY: .word 0
_activate_kernel_restore: .word activate_kernel_restore

.type activate_kernel, %function
.global activate_kernel
.global activate_kernel_restore
activate_kernel:
	cmp r0, #0
	beq load
save:
	push {r0-r10, fp, ip, lr}
	mrs ip, CPSR
	ldr lr, _activate_kernel_restore
	push {ip, lr}
	str sp, [r0, #0x0]

load:
	ldr sp, [r1, #0x0]

	pop {ip, lr}
	msr SPSR, ip
	movs pc, lr

activate_kernel_restore:
	; Discard stored on stack r0
	; Preserve prev which is stored in r0 and return it from function
	pop {r1}
	pop {r1-r10, fp, ip, lr}
	bx lr

.type activate, %function
.global activate
activate:
	ldmfd r0!, {ip,lr}
	msr SPSR, ip

	mov sp, r1

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
	stmfd r1!, {ip,lr}

	push {r1}
	bl handle_svc
	pop {r0}
	bl irq_return

.global irq_entry
irq_entry:
	push {r0-r5,ip}
	mrs ip, SPSR
	sub lr, lr, #0x4
	push {ip,lr}

	msr CPSR_c, #0xD3 /* Supervisor mode */

	ldr r0, GIC0
	ldr r0, [r0, #0x00C]

	ldr r5, =irq_level
	ldr r4, [r5]
	add r4, r4, #1
	str r4, [r5]

	cpsie i

	bl handle_irq

	cpsid i

	msr CPSR_c, #0xD2 /* IRQ mode */
	pop {ip, lr}
	msr SPSR, ip

	sub r4, r4, #1
	str r4, [r5]
	cmp r4, #0
	popne {r0-r5,ip}
	bxne lr

	ldr r5, =flags
	ldr r5, [r5]
	ands r5, r5, #1 /* NEED_RESCHED */
	pop {r0-r5,ip}
	bxeq lr

	/* Resave state on user stack */
	msr CPSR_c, #0xDF /* System mode */
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
	mov r0, sp
	msr CPSR_c, #0xD2 /* IRQ mode */

	mrs ip, SPSR
	stmfd r0!, {ip,lr}

	msr CPSR_c, #0xD3 /* Supervisor mode */
	bl irq_return
