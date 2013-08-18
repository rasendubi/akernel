CC=arm-unknown-linux-gnueabi-gcc

CFLAGS=-ansi -pedantic -Wall -Wextra -march=armv7-a -msoft-float -fPIC -mapcs-frame -I. -ffreestanding \
       -std=c99
LDFLAGS=-nostdlib -N -Tkernel.ld

QEMU=qemu-system-arm
BOARD=realview-pb-a8
CPU=cortex-a8

all: kernel.elf

kernel.elf: kernel.ld bootstrap.o kernel.o uart.o context_switch.o syscalls.o gic.o user.o \
		scheduler.o pipe.o page_alloc.o svc.o svc_entries.o alloc.o

run: kernel.elf
	$(QEMU) -M $(BOARD) -cpu $(CPU) -nographic -kernel kernel.elf

clean:
	rm -f *.o *.elf

.SUFFIXES: .o .elf
.o.elf:
	$(CC) $(LDFLAGS) -o $@ $^ -lgcc -lc

.SUFFIXES: .s .o
.s.o:
	$(CC) $(CFLAGS) -o $@ -c $^
