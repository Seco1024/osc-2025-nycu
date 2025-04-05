GNU-TOOLCHAIN-PREFIX = aarch64-linux-gnu
CC = $(GNU-TOOLCHAIN-PREFIX)-gcc
LD = $(GNU-TOOLCHAIN-PREFIX)-ld
OBJCPY = $(GNU-TOOLCHAIN-PREFIX)-objcopy

CFLAGS = -Iinclude -Wall -ffreestanding -nostdlib -nostartfiles -Iinclude -g -mgeneral-regs-only
ENV = -initrd ./bootloader/initramfs.cpio -dtb ./archive/bcm2710-rpi-3-b-plus.dtb

BUILD_DIR = build

KERNEL_C_SRCS   = $(wildcard kernel/*.c)
KERNEL_S_SRCS   = $(wildcard kernel/*.S)
DRIVERS_C_SRCS  = $(wildcard drivers/*.c)
LIB_C_SRCS      = $(wildcard lib/*.c)

C_SOURCES       = $(KERNEL_C_SRCS) $(DRIVERS_C_SRCS) $(LIB_C_SRCS)
S_SOURCES       = $(KERNEL_S_SRCS)

C_OBJECTS       = $(patsubst %.c, $(BUILD_DIR)/%.o, $(C_SOURCES))
S_OBJECTS       = $(patsubst %.S, $(BUILD_DIR)/%.o, $(S_SOURCES))
OBJ_FILES       = $(C_OBJECTS) $(S_OBJECTS)


all: kernel8.img

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.S
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

kernel8.img: kernel/linker.ld $(OBJ_FILES)
	$(LD) -T kernel/linker.ld -o $(BUILD_DIR)/kernel8.elf $(OBJ_FILES)
	$(OBJCPY) -O binary $(BUILD_DIR)/kernel8.elf kernel8.img

run:
	qemu-system-aarch64 -M raspi3b -kernel kernel8.img -display none -serial null -serial stdio ${ENV}

debug:
	qemu-system-aarch64 -M raspi3b -kernel kernel8.img -display none -serial null -serial stdio -d in_asm -s -S ${ENV}

clean:
	rm -rf $(BUILD_DIR) kernel8.img

.PHONY: all clean