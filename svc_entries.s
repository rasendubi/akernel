.global svc_handlers
.global end_svcs
svc_handlers:
.word handle_yield
.word handle_fork
.word handle_getpid
.word handle_write
.word handle_read
.word handle_register_isr
.word handle_page_alloc
.word handle_page_free
end_svcs:
