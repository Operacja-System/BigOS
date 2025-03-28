#include <debug/debug_stdio.h>
#include <stdarg.h>
#include <stdbigos/stdio.h>

void dprintf(char* (*handler)(const char*, void*, int), const char* fmt, ...) {
	va_list va;
	va_start(va, fmt);
	char buf[STB_SPRINTF_MIN];
	vsprintfcb(handler, buf, buf, fmt, va);
	va_end(va);
}
