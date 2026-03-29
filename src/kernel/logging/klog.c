#include "klog.h"

#include <../../external/include/stb_sprintf.h>
#include <stdarg.h>
#include <stdbigos/types.h>

#include "debug/debug_stdio.h"

static u32 g_indent_level = 0;
static const char* g_prefixes[] = {"[ERROR]", "[WARNING]", "[ ]", "[~]"};
static const char* g_overflow_warning = "[WARNING]Some of the logs were lost due to overflow";

#ifdef __DEBUG__
	#define RING_BUF_SIZE (1 << 20)
#else
	#define RING_BUF_SIZE (1 << 16)
#endif // !__DEBUG__

#define TEMP_BUF_SIZE (1 << 12)

typedef struct {
	char ring[RING_BUF_SIZE];
	i32 read;
	i32 write;
	bool full;
	bool overflown;
	void (*uart_tx)(char c);
} ring_buffer_t;

static ring_buffer_t g_buffer;

static ring_buffer_t* get_buffer() {
	return &g_buffer;
}

static void put_char(ring_buffer_t* buf, const char c) {
	if (c == '\0')
		return; // Null characters are not suppossed to be logged.
	buf->ring[buf->write++] = c;
	buf->write %= RING_BUF_SIZE;
	if (buf->full) {
		buf->read = (buf->read + 1) % RING_BUF_SIZE;
		buf->overflown = true;
	}
	buf->full = buf->read == buf->write;
	if (buf->uart_tx)
		buf->uart_tx(c);
}

static void put_string(ring_buffer_t* buf, const char* str) {
	size_t it = 0;
	while (str[it]) put_char(buf, str[it++]);
}

static void put_overflow_warning(ring_buffer_t* buf) {
	size_t it = 0;
	i32 temp_write = buf->write;
	while (g_overflow_warning[it]){
		buf->ring[temp_write++] = g_overflow_warning;
		temp_write %= RING_BUF_SIZE;
	}
}

static void put_msg(ring_buffer_t* buf, const char* msg) {
	put_string(buf, msg);
	if (buf->overflown)
		put_overflow_warning(buf);
}

static char get_char(ring_buffer_t* buf) {
	if (buf->read == buf->write && !buf->full)
		return 0;
	const char ret = buf->ring[buf->read++];
	buf->read %= RING_BUF_SIZE;
	buf->full = false;
	return ret;
}

void flush_to_uart() {
	ring_buffer_t* buf = get_buffer();
	if (!buf->uart_tx)
		return;
	char c;
	while ((c = get_char(buf))) buf->uart_tx(c);
}

void set_uart_tx_function(void (*uart_tx)(char c), bool flush) {
	ring_buffer_t* buf = get_buffer();
	buf->uart_tx = uart_tx;
	if (flush)
		flush_to_uart();
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
	for (u32 i = 0; i < g_indent_level && written < TEMP_BUF_SIZE - 1; i++) temp_buffer[written++] = ' ';

	const char* prefix = g_prefixes[loglvl];
	for (size_t i = 0; prefix[i] && written < TEMP_BUF_SIZE - 1; i++) temp_buffer[written++] = prefix[i];

	va_list copy;
	va_copy(copy, va);
	int n = stbsp_vsnprintf(temp_buffer + written, TEMP_BUF_SIZE - written, fmt, copy);
	va_end(copy);

	// stbsp_vsnprintf doesn't return errors, so just check for truncation
	if (n < 0)
		n = 0;
	written += (size_t)n;

	if (written >= TEMP_BUF_SIZE)
		written = TEMP_BUF_SIZE - 1;
	temp_buffer[written] = '\0';

	put_msg(&g_buffer, temp_buffer);

	// DEBUG_PUTGAP(g_indent_level);
	// DEBUG_PRINTF("%s ", g_prefixes[loglvl]);
	// DEBUG_VPRINTF(fmt, va);
}

static void kloglnv(klog_severity_level_t loglvl, const char* fmt, va_list va) {
	klogv(loglvl, fmt, va);
	put_msg(&g_buffer, "\n");
	// DEBUG_PUTC('\n');
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
