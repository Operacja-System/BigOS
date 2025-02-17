OS := BigOS

CC := riscv64-unknown-elf-gcc
AC := riscv64-unknown-elf-ac
LD := riscv64-unknown-elf-ld
VM := qemu-system-riscv64
OBJDUMP := riscv64-unknown-elf-objdump
OBJCOPY := riscv64-unknown-elf-objcopy

CFLAGS := -Wall -Wextra -ffreestanding -nostdlib -g -O0
LDFLAGS := -T linker.ld -nostdlib
VMFLAGS :=

SRC_DIR := src
BUILD_DIR := build

SRCC := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/**/*.c)
SRCS := $(wildcard $(SRC_DIR)/*.s) $(wildcard $(SRC_DIR)/**/*.s)

.PHONY: all clean always image run format

all: always image

always:
	mkdir -p build

run: all
	$(VM) $(VMFLAGS) $(BUILD_DIR)/$(OS).img


format $(SRCC):
	@clang-format -i $(SRCC)

image:

clean:
	rm -rf build
