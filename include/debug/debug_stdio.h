#ifndef _DEBUG_STDIO_
#define _DEBUG_STDIO_

void dputc(char c);
void dputs(const char* s);

[[gnu::format(printf, 2, 3)]]
void dprintf(char *(*handler)(const char *, void*, int), const char* fmt,...);

[[gnu::format(printf, 1, 2)]]
void uprintf(const char *fmt, ...);

#ifndef NDEBUG

	#define DEBUG_PUTC(c)          dputc(c)
	#define DEBUG_PUTS(s)          dputs(s)
	#define DEBUG_PRINTF(fmt, ...) dprintf(fmt __VA_OPT__(, ) __VA_ARGS__)

#else

// for argument checking
[[gnu::format(printf, 1, 2)]] static inline void noop_printf(const char*, ...) {}

	#define DEBUG_PUTC(c)
	#define DEBUG_PUTS(s)
	#define DEBUG_PRINTF(fmt, ...) noop_printf(fmt __VA_OPT__(, ) __VA_ARGS__)

#endif
#endif
