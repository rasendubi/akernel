CC=arm-unknown-linux-gnueabi-gcc
OBJCOPY=arm-unknown-linux-gnueabi-objcopy

CFLAGS=-ansi -pedantic -Wall -Wextra -march=armv7-a -msoft-float -fPIE -mapcs-frame -I. -ffreestanding \
       -std=c99
LDFLAGS=-nostdlib -N
LIBS=-lgcc -lc

QEMU=qemu-system-arm
BOARD=realview-pb-a8
CPU=cortex-a8

all: kernel.elf

kernel.elf: kernel.ld bootstrap.o kernel.o uart.o context_switch.o gic.o user.o \
		scheduler.o pipe.o page_alloc.o svc.o svc_entries.o alloc.o print.o irq.o \
		growbuf.o user_pipe_master.o ramdisk.o exec_elf.o tarfs.o \
		exec.o user/syscalls.o slab.o slab_alloc.o
	$(CC) $(LDFLAGS) -o $@ $^ -lgcc -lc -Tkernel.ld

ramdisk.o: ramdisk.tar
	$(OBJCOPY) -I binary -O elf32-littlearm -B arm $^ $@ --rename-section .data=ramdisk

ramdisk.tar: \
		stupid \
		user/services/pipe_master \
		user/alloc_test
	tar cf $@ $^

stupid: stupid.o uart.o

user/services/pipe_master: user/services/pipe_master.o user/syscalls.o user/page_alloc.o alloc.o print.o uart.o

user/alloc_test: user/alloc_test.o alloc.o print.o uart.o user/page_alloc.o user/syscalls.o

run: kernel.elf
	$(QEMU) -M $(BOARD) -cpu $(CPU) -nographic -kernel kernel.elf

clean:
	rm -f *.o *.elf *.tar \
		stupid \
		user/*.o \
		user/alloc_test \
		user/services/*.o \
		user/services/pipe_master \

%: %.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.s
	$(CC) $(CFLAGS) -o $@ -c $^
