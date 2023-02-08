#makefile for kernel testing. Just for fun ;)

CC	= clang
CFLAGS	= --target=i686-pc-none-elf -march=i686 \
	  -ffreestanding -fno-builtin -nostdlib \
	  -nostdinc -Iinclude -Wall -Wextra
AS	= i686-linux-gnu-as
LDFLAGS	= -Ttext 0x2000  -e start32 -T linker.ld \
	  --target=i686-pc-none-elf -march=i686 \
	  -ffreestanding -fno-builtin -nostdlib
BIN	= myos.bin
OBJ	= main.o setups.o printk.o vsprintf.o
SRC	= main.c setups.c printk.c vsprintf.c

kernel:	$(OBJ) boot.o start32.o
	$(CC) boot.o start32.o $(OBJ) $(LDFLAGS) -o myos.bin

boot.o:	boot.s
	i686-linux-gnu-as boot.s -o boot.o

start32:start32.s
	i686-linux-gnu-as start32.s -o start32.o

clean:
	rm -f *.o myos.bin 

run:	
	qemu-system-i386 -kernel myos.bin

iso:	
	cp myos.bin isodir/boot/myos.bin
	grub-mkrescue -o myos.iso isodir
