.macro syscall n, name
.type \name, %function
.global \name
\name:
	push {r7}
	mov r7, \n
	svc 0
	pop {r7}
	bx lr
.endm

syscall #0x00, yield
syscall #0x01, fork
syscall #0x02, getpid
syscall #0x03, write
syscall #0x04, read
syscall #0x05, sys_register_isr
syscall #0x06, sys_page_alloc
syscall #0x07, sys_page_free
