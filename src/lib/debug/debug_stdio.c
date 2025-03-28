#include <debug/debug_stdio.h>
#include <drivers/uart.h>
#include <stdarg.h>
#include <stdbigos/stdio.h>
#include <stdbigos/string.h>

void dprintf(char* (*handler)(const char*, void*, int), const char* fmt, ...) {
	va_list va;
	va_start(va, fmt);
	char buf[STB_SPRINTF_MIN];
	vsprintfcb(handler, buf, buf, fmt, va);
	va_end(va);
}

static char* uart_output_handler(const char* buf, void* user, int len) {
	while (len--) putc_uart(*buf++);
	return (char*)user;
}

// by u i mean "uart"
void uprintf(const char* fmt, ...) {
	va_list va;
	va_start(va, fmt);
	char buf[STB_SPRINTF_MIN];
	vsprintfcb(handler, buf, buf, fmt, va);
	va_end(va);
}

static char* uart_output_handler(const char* buf, void* user, int len) {
	while (len--) putc_uart(*buf++);
	return (char*)user;
}

// by u i mean "uart"
void uprintf(const char* fmt, ...) {
	va_list va;
	va_start(va, fmt);
	char buf[STB_SPRINTF_MIN];
	vsprintfcb(uart_output_handler, buf, buf, fmt, va);
	va_end(va);
}
