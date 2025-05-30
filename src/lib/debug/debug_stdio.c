#include <debug/debug_stdio.h>
#include <stdarg.h>
#include <stdbigos/stdio.h>

static volatile unsigned char* uart = (volatile unsigned char*)0x10000000;

void dputc(char c) {
	*uart = c;
}

void dputs(const char* s) {
	while (*s) dputc(*s++);
}

void dputgap(unsigned int gap_size) {
	while (gap_size--) dputc('\t');
}

static char* uart_output_handler(const char* buf, void* user, int len) {
	while (len--) dputc(*buf++);
	return (char*)user;
}

void dprintf(const char* fmt, ...) {
	va_list va;
	va_start(va, fmt);
	char buf[STB_SPRINTF_MIN];
	vsprintfcb(handler, buf, buf, fmt, va);
	va_end(va);
}

static char* uart_output_handler(const char* buf, void* user, int len) {
	while (len--) dputc(*buf++);
	return (char*)user;
}

void dprintf(const char* fmt, ...) {
	va_list va;
	va_start(va, fmt);
	char buf[STB_SPRINTF_MIN];
	vsprintfcb(uart_output_handler, buf, buf, fmt, va);
	va_end(va);
}