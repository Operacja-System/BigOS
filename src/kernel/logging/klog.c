#include "klog.h"

#include <stdarg.h>
#include <stdbigos/stdio.h>
#include <stdbigos/types.h>

#include "debug/debug_stdio.h"

static u32 g_indent_level = 0;
static const char* g_prefixes[] = {"[ERROR] ", "[WARNING] ", "[ ] ", "[~] "};
static const char* g_overflow_warning = "[WARNING] Some of the logs were lost due to overflow";

#ifdef __DEBUG__
const size_t RING_BUF_SIZE = 1 << 20;
#else
const size_t RING_BUF_SIZE = 1 << 16;
#endif // !__DEBUG__

const size_t TEMP_BUF_SIZE = 1 << 12;

typedef struct {
	char ring[RING_BUF_SIZE];
	i32 read;
	i32 write;
	bool full;
	bool overflown;
	void (*serial_tx)(char c);
} ring_buffer_t;

static ring_buffer_t g_buffer;

static void put_char(ring_buffer_t* rb, const char c) {
	if (c == '\0')
		return;                                    // Null characters are not suppossed to be logged.
	rb->ring[rb->write++] = (c == '\t' ? ' ' : c); // '\t' char is apparently wasteful
	rb->write %= RING_BUF_SIZE;
	if (rb->full) {
		rb->read = (rb->read + 1) % RING_BUF_SIZE;
		rb->overflown = true;
	}
	rb->full = rb->read == rb->write;
	if (rb->serial_tx)
		rb->serial_tx(c);
}

static void put_string(ring_buffer_t* rb, const char* str) {
	size_t it = 0;
	while (str[it]) put_char(rb, str[it++]);
}

static void put_overflow_warning() {
	size_t it = 0;
	while (g_overflow_warning[it]) g_buffer.serial_tx(g_overflow_warning[it++]);
}

static char ring_buffer_get_char() {
	if (g_buffer.read == g_buffer.write && !g_buffer.full)
		return 0;
	const char ret = g_buffer.ring[g_buffer.read++];
	g_buffer.read %= RING_BUF_SIZE;
	g_buffer.full = false;
	g_buffer.overflown = false;
	return ret;
}

void klog_flush_serial() {
	if (!g_buffer.serial_tx)
		return;
	if (g_buffer.overflown)
		put_overflow_warning();
	char c;
	while ((c = ring_buffer_get_char())) g_buffer.serial_tx(c);
}

void klog_set_serial_handler(void (*serial_tx)(char c), bool flush) {
	g_buffer.serial_tx = serial_tx;
	if (flush)
		klog_flush_serial();
}

void klog_indent_increase() {
	++g_indent_level;
}

void klog_indent_decrease() {
	if (g_indent_level != 0)
		--g_indent_level;
}

static void klogv(klog_severity_level_t loglvl, const char* fmt, va_list va) {
	char temp_buffer[TEMP_BUF_SIZE];

	if (loglvl > KLSL_TRACE) {
		KLOGLN_ERROR("Invalid loglvl passed to klog");
		loglvl = KLSL_ERROR;
	}

	size_t written = 0;
	for (u32 i = 0; i < g_indent_level && written < TEMP_BUF_SIZE - 1; i++)
		temp_buffer[written++] = (char)(g_indent_level > 8 ? 8 : g_indent_level);

	const char* prefix = g_prefixes[loglvl];
	for (size_t i = 0; prefix[i] && written < TEMP_BUF_SIZE - 1; i++) temp_buffer[written++] = prefix[i];

	va_list copy;
	va_copy(copy, va);
	int n = vsnprintf(temp_buffer + written, TEMP_BUF_SIZE - written, fmt, copy);
	va_end(copy);

	// stbsp_vsnprintf doesn't return errors, so just check for truncation
	if (n < 0)
		n = 0;
	written += (size_t)n;

	if (written >= TEMP_BUF_SIZE)
		written = TEMP_BUF_SIZE - 1;
	temp_buffer[written] = '\0';

	put_string(&g_buffer, temp_buffer);
}

static void kloglnv(klog_severity_level_t loglvl, const char* fmt, va_list va) {
	klogv(loglvl, fmt, va);
	put_string(&g_buffer, "\n");
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
