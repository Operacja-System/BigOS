#include "klog.h"

#include <debug/debug_stdio.h>
#include <libcore/types.h>

static u32 g_indent_level = 0;
static const char* g_prefixes[] = {"[ERROR]", "[WARNING]", "[ ]", "[~]"};

void klog_indent_increase() {
	++g_indent_level;
}

void klog_indent_decrease() {
	if (g_indent_level != 0)
		--g_indent_level;
}

static void klogv(klog_severity_level_t loglvl, const char* fmt, va_list va) {
	(void)fmt;
	(void)va;

	if (loglvl > KLSL_TRACE) {
		KLOGLN_ERROR("Invalid loglvl passed to klog");
		loglvl = KLSL_ERROR;
	}

	DEBUG_PUTGAP(g_indent_level);
	DEBUG_PRINTF("%s ", g_prefixes[loglvl]);
	DEBUG_VPRINTF(fmt, va);
}

static void kloglnv(klog_severity_level_t loglvl, const char* fmt, va_list va) {
	klogv(loglvl, fmt, va);
	DEBUG_PUTC('\n');
}

void klog(klog_severity_level_t loglvl, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	klogv(loglvl, fmt, args);
	va_end(args);
}

void klogln(klog_severity_level_t loglvl, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	kloglnv(loglvl, fmt, args);
	va_end(args);
}
