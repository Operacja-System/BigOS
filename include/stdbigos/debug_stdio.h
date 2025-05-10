#ifndef _DEBUG_STDIO_
#define _DEBUG_STDIO_

#include <stdarg.h>
#include <stdio.h>

#ifndef NDEBUG

static inline void dputc(int c) {
	putchar(c);
}
static inline void dputs(char* s) {
	puts(s);
}
[[gnu::format(printf, 1, 2)]]
static inline void dprintf(char* fmt, ...) {
	va_list va;
	va_start(va);
	vprintf(fmt, va);
	va_end(va);
}

#else

static inline void dputc(int) {}
static inline void dputs(char*) {}
[[gnu::format(printf, 1, 2)]]
static inline void dprintf(char*, ...) {}

#endif
#endif
