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

syscall #0x00, sys_yield
syscall #0x01, sys_fork
syscall #0x02, sys_getpid
syscall #0x03, sys_write
syscall #0x04, sys_read
syscall #0x05, sys_register_isr
syscall #0x06, sys_page_alloc
syscall #0x07, sys_page_free
syscall #0x08, sys_exec
syscall #0x09, sys_exit
