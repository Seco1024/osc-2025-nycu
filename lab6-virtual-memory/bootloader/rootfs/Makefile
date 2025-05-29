GNU-TOOLCHAIN-PREFIX = aarch64-linux-gnu
CC = $(GNU-TOOLCHAIN-PREFIX)-gcc
LD = $(GNU-TOOLCHAIN-PREFIX)-ld
OBJCPY = $(GNU-TOOLCHAIN-PREFIX)-objcopy

CFLAGS = -Iinclude -Wall -ffreestanding -nostdlib -nostartfiles -Iinclude -g -mgeneral-regs-only

BUILD_DIR = build

C_SOURCES       = $(wildcard *.c)
S_SOURCES       = $(wildcard *.S)

C_OBJECTS       = $(patsubst %.c, $(BUILD_DIR)/%.o, $(C_SOURCES))
S_OBJECTS       = $(patsubst %.S, $(BUILD_DIR)/%.o, $(S_SOURCES))
OBJ_FILES       = $(C_OBJECTS) $(S_OBJECTS)

all: test_svc.img

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.S
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

test_svc.img: linker.ld $(OBJ_FILES)
	$(LD) -T linker.ld -o $(BUILD_DIR)/test_svc.elf $(OBJ_FILES)
	$(OBJCPY) -O binary $(BUILD_DIR)/test_svc.elf test_svc.img

clean:
	rm -rf $(BUILD_DIR) test_svc.img

.PHONY: all clean