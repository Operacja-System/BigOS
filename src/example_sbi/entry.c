#include <stdbigos/sbi.h>
#include <stdbigos/types.h>

void sbi_puts(const char* str) {
	while (*str) sbi_debug_console_write_byte(*str++);
}

void main(u32, const void*) {
	sbi_puts("Hello world\n");
}
