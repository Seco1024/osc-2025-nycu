GNU-TOOLCHAIN-PREFIX = aarch64-linux-gnu
CC = $(GNU-TOOLCHAIN-PREFIX)-gcc
LD = $(GNU-TOOLCHAIN-PREFIX)-ld
OBJCPY = $(GNU-TOOLCHAIN-PREFIX)-objcopy

CFLAGS = -Iinclude -Wall -ffreestanding -nostdlib -nostartfiles -Iinclude -g -mgeneral-regs-only
ENV = -initrd initramfs.cpio -dtb ./archive/bcm2710-rpi-3-b.dtb

BUILD_DIR = build

SRC_C_SRCS   = $(wildcard src/*.c)
SRC_S_SRCS   = $(wildcard src/*.S)
DRIVERS_C_SRCS  = $(wildcard drivers/*.c)
LIB_C_SRCS      = $(wildcard lib/*.c)

C_SOURCES       = $(SRC_C_SRCS) $(DRIVERS_C_SRCS) $(LIB_C_SRCS)
S_SOURCES       = $(SRC_S_SRCS)

C_OBJECTS       = $(patsubst %.c, $(BUILD_DIR)/%.o, $(C_SOURCES))
S_OBJECTS       = $(patsubst %.S, $(BUILD_DIR)/%.o, $(S_SOURCES))
OBJ_FILES       = $(C_OBJECTS) $(S_OBJECTS)


all: bootloader.img

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.S
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

bootloader.img: src/linker.ld $(OBJ_FILES)
	$(LD) -T src/linker.ld -o $(BUILD_DIR)/bootloader.elf $(OBJ_FILES)
	$(OBJCPY) -O binary $(BUILD_DIR)/bootloader.elf bootloader.img

run:
	qemu-system-aarch64 -M raspi3b -kernel bootloader.img -display none -serial null -serial stdio ${ENV}

pty:
	qemu-system-aarch64 -M raspi3b -kernel bootloader.img -display none -serial null -serial pty ${ENV}

debug:
	qemu-system-aarch64 -M raspi3b -kernel bootloader.img -display none -serial null -serial stdio -d in_asm -s -S ${ENV}

clean:
	rm -rf $(BUILD_DIR) bootloader.img

.PHONY: all clean