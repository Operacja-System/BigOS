find_tool = $(shell (command -v riscv64-unknown-elf-$(1) 2>/dev/null) || \
                    (command -v riscv64-elf-$(1) 2>/dev/null) || \
                    (command -v riscv64-none-elf-$(1) 2>/dev/null) || \
                    (echo ""))

export RV_CC      := $(call find_tool,gcc)
export RV_AS      := $(call find_tool,as)
export RV_LD      := $(call find_tool,ld)
export RV_OBJDUMP := $(call find_tool,objdump)
export RV_OBJCOPY := $(call find_tool,objcopy)
export RV_GDB     := $(call find_tool,gdb)

ifeq ($(CC),)
  $(error "No valid RISC-V compiler found! Please install one or set CC manually.")
endif

INCLUDES := lib
DEFINES := DEBUG

export CFLAGS := -Wall -Wextra -Werror -mcmodel=medany -g -O0 -Wno-ignored-qualifiers -ffreestanding -march=rv64g -std=c23
CFLAGS += $(addprefix -D__, $(addsuffix __,$(DEFINES)))
CFLAGS += $(patsubst %, -I$(SRC_DIR)/%, $(INCLUDES))
export AFLAGS := -march=rv64g
export LDFLAGS := -nostdlib

SRC_DIRS := $(wildcard $(SRC_DIR)/*/)
SRC_DIRS := $(filter-out $(SRC_DIR)/lib/, $(SRC_DIRS))

all:
	make -C $(SRC_DIR)/lib/ SRC_DIR=$(SRC_DIR) BUILD_DIR=$(BUILD_DIR)
	@for dir in $(SRC_DIRS); do make -C $$dir SRC_DIR=$(SRC_DIR) BUILD_DIR=$(BUILD_DIR); done

.PHONY: all
