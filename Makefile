OS := BigOS

export CC := riscv64-elf-gcc
export AS := riscv64-elf-as
export LD := riscv64-elf-ld
export VM := qemu-system-riscv64
export OBJDUMP := riscv64-elf-objdump
export OBJCOPY := riscv64-elf-objcopy

INCLUDES := -I./src

DEFINES := DEBUG GCC
export CFLAGS := -Wall -Wextra -Werror -ffreestanding -nostdlib -nolibc -mcmodel=medany -g -O0 -Wno-ignored-qualifiers
CFLAGS += $(addprefix -D__, $(addsuffix __,$(DEFINES)))
CFLAGS += $(INCLUDES)
export AFLAGS := -g
export LDFLAGS := -nostdlib
VMFLAGS := -bios none -machine virt -serial mon:stdio

SRC_DIR := src
BUILD_DIR := build

.PHONY: all clean always image run format 

SRC_DIRS := $(wildcard $(SRC_DIR)/*/)
SRC_DIRS := $(filter-out $(SRC_DIR)/slib/, $(SRC_DIRS))
 
all: always 
	make -C $(SRC_DIR)/slib/ SRC_DIR=$(abspath $(SRC_DIR)) BUILD_DIR=$(abspath $(BUILD_DIR))
	@for dir in $(SRC_DIRS); do make -C $$dir SRC_DIR=$(abspath $(SRC_DIR)) BUILD_DIR=$(abspath $(BUILD_DIR)); done

always:
	mkdir -p build

run: all
	$(VM) $(VMFLAGS) -kernel $(BUILD_DIR)/example/example.elf

FILES_C := $(shell find . -name '*.c')
FILES_H := $(shell find . -name '*.h')

format $(SRCC):
	@clang-format -i $(FILES_C)
	@clang-format -i $(FILES_H)

clean:
	rm -rf build

image: 
	dd if=/dev/zero of=$(BUILD_DIR)/$(OS).img bs=1M count=10
