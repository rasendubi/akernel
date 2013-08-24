CC=arm-unknown-linux-gnueabi-gcc
OBJCOPY=arm-unknown-linux-gnueabi-objcopy

CFLAGS=-ansi -pedantic -Wall -Wextra -march=armv7-a -msoft-float -fPIE -mapcs-frame -I. -ffreestanding \
       -std=c99
LDFLAGS=-nostdlib -N -Tkernel.ld

QEMU=qemu-system-arm
BOARD=realview-pb-a8
CPU=cortex-a8

all: kernel.elf

kernel.elf: kernel.ld bootstrap.o kernel.o uart.o context_switch.o syscalls.o gic.o user.o \
		scheduler.o pipe.o page_alloc.o svc.o svc_entries.o alloc.o print.o irq.o \
		growbuf.o pipe_master.o user_pipe_master.o ramdisk.o exec_elf.o

ramdisk.o: ramdisk.tar
	$(OBJCOPY) -I binary -O elf32-littlearm -B arm $^ $@ --rename-section .data=.ramdisk

ramdisk.tar: stupid
	tar cf $@ $^

stupid: stupid.o uart.o

run: kernel.elf
	$(QEMU) -M $(BOARD) -cpu $(CPU) -nographic -kernel kernel.elf

clean:
	rm -f *.o *.elf *.tar

.SUFFIXES: .o .elf
.o.elf:
	$(CC) $(LDFLAGS) -o $@ $^ -lgcc -lc

.SUFFIXES: .s .o
.s.o:
	$(CC) $(CFLAGS) -o $@ -c $^
