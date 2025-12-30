CROSS   ?= i686-elf-

CC      = $(CROSS)gcc
AS      = $(CROSS)as
LD      = $(CROSS)ld
OBJCOPY = $(CROSS)objcopy

CFLAGS  = -m32 -march=i686 -ffreestanding -fno-builtin -fno-pie -no-pie \
          -nostdlib -nostdinc -Iinclude -Wall -Wextra

# Kernelin linkitys: pidä latausosoite linker.ld:ssä (. = 0x2000;)
LDFLAGS = -m elf_i386 -T linker.ld -e start32

ELF     = myos.elf
BIN     = myos.bin

BOOTSECT = bootsect.bin
FLOPPY   = floppy.img
FLOPPY_SIZE = 1474560

OBJ     = main.o setups.o printk.o vsprintf.o

KSECTORS ?= 21
KMAX := $(shell echo $$(( $(KSECTORS) * 512 )))
.PHONY: all clean run floppy

all: $(FLOPPY)

#
# 1) Bootsektori legacy-koodista (512B, 0x7C00)
#
$(BOOTSECT): boot_legacy.s
	$(AS) --32 $< -o boot_legacy.o
	$(LD) -m elf_i386 -Ttext 0x0 --oformat=binary -o $@ boot_legacy.o
	@# pakota tarkistus: pitää olla täsmälleen 512
	@test "$$(stat -c %s $(BOOTSECT))" -eq 512

#
# 2) Kernel ELF -> BIN
#
$(ELF): start32.o $(OBJ) linker.ld
	$(LD) $(LDFLAGS) -o $@ start32.o $(OBJ)

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $(ELF) $(BIN)

start32.o: start32.s
	$(AS) --32 $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

#
# 3) Floppy image: sector0=boot, sector1..=kernel
#
$(FLOPPY): check-kernel-size $(BOOTSECT) $(BIN)
	dd if=/dev/zero of=$(FLOPPY) bs=1 count=$(FLOPPY_SIZE) status=none
	dd if=$(BOOTSECT) of=$(FLOPPY) conv=notrunc bs=512 seek=0 status=none
	dd if=$(BIN)      of=$(FLOPPY) conv=notrunc bs=512 seek=1 status=none

run: $(FLOPPY)
	qemu-system-i386 -drive file=$(FLOPPY),format=raw,if=floppy -boot a

clean:
	rm -f *.o $(ELF) $(BIN) $(BOOTSECT) $(FLOPPY)

check-kernel-size: $(BIN)
	@test "$$(stat -c %s $(BIN))" -le "$(KMAX)" || \
	 (echo "ERROR: $(BIN) too big (max $(KMAX) bytes for $(KSECTORS) sectors)"; exit 1)

