find_tool = $(shell (command -v riscv64-unknown-elf-$(1) 2>/dev/null) || \
                    (command -v riscv64-elf-$(1) 2>/dev/null) || \
                    (command -v riscv64-none-elf-$(1) 2>/dev/null) || \
                    (echo ""))

RV_CC      := $(call find_tool,gcc)
RV_AS      := $(call find_tool,as)
RV_LD      := $(call find_tool,ld)
RV_OBJDUMP := $(call find_tool,objdump)
RV_OBJCOPY := $(call find_tool,objcopy)
RV_GDB     := $(call find_tool,gdb)

ifeq ($(RV_CC),)
  $(error "No valid RISC-V compiler found! Please install one or set CC manually.")
endif
