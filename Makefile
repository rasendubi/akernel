CC=arm-unknown-linux-gnueabi-gcc
LD=arm-unknown-linux-gnueabi-ld

CFLAGS=-ansi -pedantic -Wall -Wextra -march=armv7-a -msoft-float -fPIC -mapcs-frame -I.
LDFLAGS=-N -Ttext=0x10000

QEMU=qemu-system-arm
BOARD=realview-pb-a8
CPU=cortex-a8

all: kernel.elf

kernel.elf: bootstrap.o kernel.o uart.o context_switch.o syscalls.o gic.o

run: kernel.elf
	$(QEMU) -M $(BOARD) -cpu $(CPU) -nographic -kernel kernel.elf

.SUFFIXES: .o .elf
.o.elf:
	$(LD) $(LDFLAGS) -o $@ $^

.SUFFIXES: .s .o
.s.o:
	$(CC) $(CFLAGS) -o $@ -c $^
