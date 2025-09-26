#include <klog.h>
#include <stdarg.h>
#include <stdbigos/types.h>

#include "debug/debug_stdio.h"

static u32 s_ident_level = 0;

void klog_indent_increase() {
	++s_ident_level;
}

void klog_indent_decrease() {
	--s_ident_level;
}

void klog(klog_severity_level_t loglvl, unsigned int depth, const char* fmt, ...) {
#ifdef __DEBUG__
	dputgap(depth);
	static const char* prefixes[] = {"[ERROR]", "[WARNING]", "[ ]", "[~]"};
	va_list args;
	va_start(args, fmt);
	dputgap(s_ident_level);
	dprintf("%s ", prefixes[loglvl]);
	dvprintf(fmt, args);
	va_end(args);
#else
	(void)loglvl;
	(void)depth;
	(void)fmt;
#endif
}

void klogln(klog_severity_level_t loglvl, unsigned int depth, const char* fmt, ...) {
#ifdef __DEBUG__
	dputgap(depth);
	static const char* prefixes[] = {"[ERROR]", "[WARNING]", "[ ]", "[~]"};
	va_list args;
	va_start(args, fmt);
	dputgap(s_ident_level);
	dprintf("%s ", prefixes[loglvl]);
	dvprintf(fmt, args);
	dputc('\n');
	va_end(args);
#else
	(void)loglvl;
	(void)depth;
	(void)fmt;
#endif
}
