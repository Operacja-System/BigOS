#ifndef _DEBUG_STDIO_
#define _DEBUG_STDIO_

#include <stdarg.h>

void dputc(char c);
void dputs(const char* s);
void dputgap(unsigned int gap_size);

[[gnu::format(printf, 1, 2)]]
void dprintf(const char* fmt, ...);
void dvprintf(const char* fmt, va_list args);

#ifndef NDEBUG

	#define DEBUG_PUTC(c)          dputc(c)
	#define DEBUG_PUTS(s)          dputs(s)
	#define DEBUG_PRINTF(fmt, ...) dprintf(fmt __VA_OPT__(, ) __VA_ARGS__)
	#define DEBUG_PUTGAP(n)        dputgap(n)

#else

// for argument checking
[[gnu::format(printf, 1, 2)]] static inline void noop_printf(const char*, ...) {}

	#define DEBUG_PUTC(c)
	#define DEBUG_PUTS(s)
	#define DEBUG_PRINTF(fmt, ...) noop_printf(fmt __VA_OPT__(, ) __VA_ARGS__)
	#define DEBUG_PUTGAP(n)

#endif
#endif
