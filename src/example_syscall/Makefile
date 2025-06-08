TARGET_PREFIX = riscv64-unknown-elf
ARCH_FLAGS = -march=rv64gc -mabi=lp64d

CC = $(TARGET_PREFIX)-gcc
AS = $(TARGET_PREFIX)-as
LD = $(TARGET_PREFIX)-ld
OBJCOPY = $(TARGET_PREFIX)-objcopy
OBJDUMP = $(TARGET_PREFIX)-objdump
GDB = $(TARGET_PREFIX)-gdb

CFLAGS = $(ARCH_FLAGS) -mcmodel=medany -Wall -Wextra -O2 -ffreestanding -Wno-unused-parameter -nostdlib -g -I.
ASFLAGS = $(ARCH_FLAGS) -mcmodel=medany -g

C_SOURCES = setup.c syscall.c test.c trap_handler.c utils.c
S_SOURCES = trap_entry.S

OBJS = $(C_SOURCES:.c=.o) $(S_SOURCES:.S=.o)

ELF_FILE = firmware.elf
MAP_FILE = firmware.map
OBJDUMP_FILE = firmware.dis

all: $(ELF_FILE)

$(ELF_FILE): $(OBJS) linker.ld
	$(CC) $(CFLAGS) -T linker.ld -o $@ $(OBJS) -nostdlib -Wl,-Map=$(MAP_FILE)
	@echo "Plik ELF $(ELF_FILE) został pomyślnie utworzony."
	@echo "Mapa linkera: $(MAP_FILE)"

%.o: %.c syscall.h sbi.h utils.h trap_handler.h
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(ASFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(ELF_FILE) $(MAP_FILE) $(OBJDUMP_FILE)
	@echo "Wyczyszczono pliki projektu."

QEMU_SYSTEM = qemu-system-riscv64
QEMU_MACHINE = -machine virt
QEMU_CPU = -cpu rv64
QEMU_MEM = -m 128M
QEMU_KERNEL = -kernel $(ELF_FILE)
QEMU_DISPLAY = -nographic

run: $(ELF_FILE)
	$(QEMU_SYSTEM) $(QEMU_MACHINE) $(QEMU_CPU) $(QEMU_MEM) $(QEMU_KERNEL) $(QEMU_DISPLAY)

dump: $(ELF_FILE)
	$(OBJDUMP) -d -S $(ELF_FILE) > $(OBJDUMP_FILE)

.PHONY: all clean run dump
