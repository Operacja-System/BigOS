#include <debug/debug_stdio.h>
#include <stdarg.h>
#include <stdbigos/stdio.h>

static volatile unsigned char* uart = (volatile unsigned char*)0x10000000;

void debug_putc(char c) {
	*uart = c;
}

void debug_puts(const char* s) {
	while(*s) debug_putc(*s++);
}

static char* uart_output_handler(const char* buf, void* user, int len) {
	while(len--) debug_putc(*buf++);
	return (char*)user;
}

void debug_printf(const char* fmt, ...) {
	va_list va;
	va_start(va, fmt);
	char buf[STB_SPRINTF_MIN];
	vsprintfcb(uart_output_handler, buf, buf, fmt, va);
	va_end(va);
}
