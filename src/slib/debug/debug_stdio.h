#ifndef _DEBUG_STDIO_
#define _DEBUG_STDIO_

void debug_putc(char c);
void debug_puts(const char* s);
void debug_printf(const char* fmt, ...);

#ifdef __DEBUG__

	#define DEBUG_PUTC(c)		   debug_putc(c)
	#define DEBUG_PUTS(s)		   debug_puts(s)
	#define DEBUG_PRINTF(fmt, ...) debug_printf(fmt, __VA_ARGS__)

#else

	#define DEBUG_PUTC(c)
	#define DEBUG_PUTS(s)
	#define DEBUG_PRINTF(fmt, ...)

#endif // !__DEBUG__

#endif // !_DEBUG_STDIO_
