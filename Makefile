OS := BigOS

SRC_DIR := src
BUILD_DIR := build

export MAKE_INCLUDE_PATH := $(shell pwd)/Make

VM := qemu-system-riscv64
VMFLAGS := -bios none -machine virt -serial mon:stdio

all:
	make -C $(SRC_DIR) SRC_DIR=$(abspath $(SRC_DIR)) BUILD_DIR=$(abspath $(BUILD_DIR))

always:
	mkdir -p build

run: all
	$(VM) $(VMFLAGS) -kernel $(BUILD_DIR)/example/example.elf

FILES_C := $(shell find . -name '*.c')
FILES_H := $(shell find . -name '*.h')

format $(FILES_C) $(FILES_H):
	@clang-format -i $(FILES_C)
	@clang-format -i $(FILES_H)

clean:
	rm -rf build

image: 
	dd if=/dev/zero of=$(BUILD_DIR)/$(OS).img bs=1M count=10

.PHONY: all clean always image run format 

