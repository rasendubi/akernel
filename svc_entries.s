.global svc_handlers
svc_handlers:
.word handle_yield
.word handle_fork
.word handle_getpid
.word handle_write
.word handle_read
