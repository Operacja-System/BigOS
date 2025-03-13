#ifdef __DEBUG__

static volatile unsigned char* uart = (volatile unsigned char*)0x10000000;

void debug_putc(char c) {
	*uart = c;
}

void debug_puts(const char* s) {
	while(*s) debug_putc(*s++);
}

void debug_printf([[maybe_unused]] const char* fmt, ...) {}

#endif // !__DEBUG__
