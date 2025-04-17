#include <stdbigos/sbi.h>
#include <stdbigos/types.h>

static void sbi_puts(const char* str) {
	while(*str) sbi_debug_console_write_byte(*str++);
}

void main([[maybe_unused]] u32 hartid, [[maybe_unused]] const void* fdt) {
	sbi_puts("Hello world\n");
}
