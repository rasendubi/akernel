.global yield
yield:
	push {r7}
	mov r7, #0x0
	svc 0
	bx lr

.global fork
fork:
	push {r7}
	mov r7, #0x1
	svc 0
	bx lr

.global getpid
getpid:
	push {r7}
	mov r7, #0x2
	svc 0
	bx lr

