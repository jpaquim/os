OS = myos
OS_BIN = $(OS).bin
OS_ISO = $(OS).iso

ISO_DIR = isodir

CC = i686-elf-gcc
AS = i686-elf-as
QEMU = qemu-system-i386

CFLAGS = -ffreestanding -O2 -Wall -Wextra
CPPFLAGS = -Iinclude
LDFLAGS = -T linker.ld -ffreestanding -O2 -nostdlib
LDLIBS = -lgcc

all: boot-bin

bin: $(OS_BIN)

iso: $(OS_ISO)

boot-bin: $(OS_BIN)
	$(QEMU) -kernel $(OS_BIN)

boot-iso: $(OS_ISO)
	$(QEMU) -cdrom $(OS_ISO)

$(OS_ISO): $(OS_BIN) grub.cfg
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(OS_BIN) $(ISO_DIR)/boot/$(OS_BIN)
	cp grub.cfg $(ISO_DIR)/boot/grub/grub.cfg
	grub-mkrescue -o $(OS_ISO) $(ISO_DIR)

OBJS = boot.o kernel.o screen.o

$(OBJS): include/*.h

$(OS_BIN): $(OBJS) linker.ld
	$(CC) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

%.o: %.s
	$(AS) -o $@ $<

clean:
	$(RM) -r $(OBJS) $(ISO_DIR) $(OS_ISO) $(OS_BIN)

.PHONY: bin iso boot boot-bin boot-iso clean
